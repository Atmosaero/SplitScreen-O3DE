/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "SplitScreenFeatureProcessor.h"

#include <Render/SplitScreenCompositePass.h>

#include <LyShine/Bus/UiCanvasBus.h>
#include <LyShine/Bus/UiTransformBus.h>
#include <Atom/RPI.Public/Pass/PassFilter.h>
#include <AzCore/Math/Uuid.h>
#include <AzCore/Math/MatrixUtils.h>
#include <AzCore/std/algorithm.h>
#include <AzFramework/Components/CameraBus.h>
#include <AzFramework/Entity/GameEntityContextBus.h>
#include <Atom/RHI.Reflect/ImagePoolDescriptor.h>
#include <Atom/RPI.Reflect/Image/AttachmentImageAssetCreator.h>
#include <Atom/RPI.Reflect/Pass/PassRequest.h>
#include <Atom/RPI.Reflect/Pass/PassTemplate.h>
#include <Atom/RPI.Reflect/ResourcePoolAssetCreator.h>
#include <Atom/RPI.Public/RPIUtils.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/View.h>
#include <Atom/RPI.Public/ViewProviderBus.h>
#include <Atom/RPI.Public/ViewportContext.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>

namespace SplitScreen
{
    void SplitScreenFeatureProcessor::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext
                ->Class<SplitScreenFeatureProcessor, FeatureProcessor>()
                ;
        }
    }

    void SplitScreenFeatureProcessor::Activate()
    {
        AZ::RPI::PassSystemInterface* passSystem = AZ::RPI::PassSystemInterface::Get();
        const AZ::Name passClassName("SplitScreenCompositePass");
        if (!passSystem->HasCreatorForClass(passClassName))
        {
            passSystem->AddPassCreator(passClassName, &SplitScreenCompositePass::Create);
        }
        AZ::TickBus::Handler::BusConnect();
    }

    void SplitScreenFeatureProcessor::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        RemoveSplitPipelines();
        m_players.clear();
    }

    void SplitScreenFeatureProcessor::Simulate([[maybe_unused]] const FeatureProcessor::SimulatePacket& packet)
    {
    }

    void SplitScreenFeatureProcessor::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (!m_rebuildPending && !m_splitPipelines.empty())
        {
            if (AZ::RPI::ViewportContextPtr viewportContext = AZ::RPI::GetDefaultViewportContext())
            {
                if (viewportContext->GetViewportSize() != m_renderSize)
                {
                    m_rebuildPending = true;
                }
            }
        }
        if (m_rebuildPending)
        {
            RebuildPipelines();
        }
        // Feature processors can inject passes after AddRenderPipeline. Suppress
        // UI in all camera pipelines every tick, including late-created passes.
        for (const auto& pipeline : m_splitPipelines)
        {
            for (const char* name : {"LyShineParentTemplate", "ImGuiPassTemplate"})
            {
                auto filter = AZ::RPI::PassFilter::CreateWithTemplateName(AZ::Name(name), pipeline.get());
                AZ::RPI::PassSystemInterface::Get()->ForEachPass(filter, [](AZ::RPI::Pass* pass)
                { pass->SetEnabled(false); return AZ::RPI::PassFilterExecutionFlow::ContinueVisitingPasses; });
            }
        }
        UpdateCameraProjections();
    }

    bool SplitScreenFeatureProcessor::RegisterLocalPlayer(AZ::EntityId entityId, bool activated)
    {
        if (!entityId.IsValid()) return false;
        auto& player = m_players[entityId];
        player.m_entityId = entityId;
        player.m_activated = activated;
        m_rebuildPending = true;
        return true;
    }

    bool SplitScreenFeatureProcessor::SetLocalPlayerCanvas(AZ::EntityId camera, AZ::EntityId canvas, uint32_t slot)
    {
        auto found = m_players.find(camera);
        if (found == m_players.end() || slot >= 4) return false;
        for (const auto& [otherCamera, other] : m_players)
        {
            if (otherCamera != camera && (other.m_slot == slot || (canvas.IsValid() && other.m_canvas == canvas)))
            {
                AZ_Warning("SplitScreen", false, "Player slots and HUD canvases must have unique owners.");
                return false;
            }
        }
        auto& player = found->second;
        if (player.m_canvas == canvas && player.m_slot == slot) return true;
        if (player.m_canvas.IsValid())
        {
            UiCanvasBus::Event(player.m_canvas, &UiCanvasInterface::SetIsRenderToTexture, player.m_wasRtt);
            UiCanvasBus::Event(player.m_canvas, &UiCanvasInterface::SetAttachmentImageAsset, player.m_previousImage);
        }
        player.m_canvas = canvas;
        player.m_slot = slot;
        if (canvas.IsValid())
        {
            UiCanvasBus::EventResult(player.m_wasRtt, canvas, &UiCanvasInterface::GetIsRenderToTexture);
            UiCanvasBus::EventResult(player.m_previousImage, canvas, &UiCanvasInterface::GetAttachmentImageAsset);
        }
        m_rebuildPending = true;
        return true;
    }

    bool SplitScreenFeatureProcessor::UnregisterLocalPlayer(AZ::EntityId entityId)
    {
        auto found = m_players.find(entityId);
        if (found == m_players.end()) return false;
        if (found->second.m_canvas.IsValid())
        {
            UiCanvasBus::Event(found->second.m_canvas, &UiCanvasInterface::SetIsRenderToTexture, found->second.m_wasRtt);
            UiCanvasBus::Event(found->second.m_canvas, &UiCanvasInterface::SetAttachmentImageAsset, found->second.m_previousImage);
        }
        m_players.erase(entityId);
        m_rebuildPending = true;
        return true;
    }

    bool SplitScreenFeatureProcessor::SetLocalPlayerActivated(AZ::EntityId entityId, bool activated)
    {
        auto player = m_players.find(entityId);
        if (player == m_players.end()) return false;
        if (player->second.m_activated != activated)
        {
            player->second.m_activated = activated;
            m_rebuildPending = true;
        }
        return true;
    }

    AZStd::vector<SplitScreenFeatureProcessor::Player> SplitScreenFeatureProcessor::GetActivePlayers() const
    {
        AZStd::vector<Player> players;
        for (const auto& [entityId, player] : m_players)
        {
            if (player.m_activated)
            {
                players.push_back(player);
            }
        }
        AZStd::sort(players.begin(), players.end(), [](const Player& left, const Player& right)
        {
            return left.m_slot == right.m_slot ? left.m_entityId < right.m_entityId : left.m_slot < right.m_slot;
        });
        if (players.size() > 4)
        {
            players.resize(4);
        }
        return players;
    }

    SplitScreenFeatureProcessor::ViewportRect SplitScreenFeatureProcessor::GetViewportRect(size_t index, size_t playerCount)
    {
        if (playerCount == 2)
        {
            return index == 0 ? ViewportRect{ 0.0f, 0.0f, 0.5f, 1.0f } : ViewportRect{ 0.5f, 0.0f, 1.0f, 1.0f };
        }

        const size_t column = index % 2;
        const size_t row = index / 2;
        return ViewportRect{
            column * 0.5f,
            row * 0.5f,
            column * 0.5f + 0.5f,
            row * 0.5f + 0.5f };
    }

    void SplitScreenFeatureProcessor::RemoveSplitPipelines()
    {
        if (m_compositePass)
        {
            m_compositePass->QueueForRemoval();
            m_compositePass = nullptr;
        }

        if (AZ::RPI::Scene* scene = GetParentScene())
        {
            for (const AZ::RPI::RenderPipelinePtr& pipeline : m_splitPipelines)
            {
                scene->RemoveRenderPipeline(pipeline->GetId());
            }
        }
        m_splitPipelines.clear();
        m_originalPipeline.reset();
        m_playerImages = {};
        m_playerImageAssets = {};
        m_hudImages = {};
        m_hudImageAssets = {};
        for (auto& [id, player] : m_players)
        {
            if (player.m_canvas.IsValid())
            {
                UiCanvasBus::Event(player.m_canvas, &UiCanvasInterface::SetIsRenderToTexture, player.m_wasRtt);
                UiCanvasBus::Event(player.m_canvas, &UiCanvasInterface::SetAttachmentImageAsset, player.m_previousImage);
            }
        }
    }

    void SplitScreenFeatureProcessor::RebuildPipelines()
    {
        m_rebuildPending = false;
        RemoveSplitPipelines();

        const AZStd::vector<Player> players = GetActivePlayers();
        if (players.size() < 2)
        {
            return;
        }

        AZ::RPI::Scene* scene = GetParentScene();
        m_originalPipeline = scene ? scene->GetDefaultRenderPipeline() : nullptr;
        AZ::RPI::ViewportContextPtr viewportContext = AZ::RPI::GetDefaultViewportContext();
        if (!scene || !m_originalPipeline || !viewportContext)
        {
            m_originalPipeline.reset();
            m_rebuildPending = true;
            return;
        }
        m_renderSize = viewportContext->GetViewportSize();
        if (m_renderSize.m_width == 0 || m_renderSize.m_height == 0)
        { m_rebuildPending = true; return; }

        AZ::RPI::PassSystemInterface* passSystem = AZ::RPI::PassSystemInterface::Get();
        const AZ::RPI::RenderPipelineDescriptor baseDescriptor = m_originalPipeline->GetDescriptor();
        if (!passSystem->HasTemplate(AZ::Name("SplitScreenCompositeTemplate")))
        {
            m_originalPipeline.reset();
            m_rebuildPending = true;
            return;
        }

        if (!m_renderTargetPoolAsset.GetId().IsValid())
        {
            AZ::RPI::ResourcePoolAssetCreator poolCreator;
            poolCreator.Begin(AZ::Data::AssetId(AZ::Uuid::CreateName("SplitScreenRenderTargetPool")));
            auto poolDescriptor = AZStd::make_unique<AZ::RHI::ImagePoolDescriptor>();
            poolDescriptor->m_bindFlags = AZ::RHI::ImageBindFlags::Color | AZ::RHI::ImageBindFlags::ShaderRead;
            poolCreator.SetPoolDescriptor(AZStd::move(poolDescriptor));
            poolCreator.SetPoolName("SplitScreenRenderTargetPool");
            if (!poolCreator.End(m_renderTargetPoolAsset))
            {
                AZ_Error("SplitScreen", false, "Failed to create the split screen render target pool.");
                return;
            }
        }

        // LyShine retains RTT draw contexts while old passes are removed. Keep
        // names unique across rebuilds so a resized/reordered HUD cannot reuse
        // another canvas's previous render scope.
        const auto generation = AZ::Uuid::CreateRandom().ToFixedString();
        for (size_t index = 0; index < players.size(); ++index)
        {
            const AZStd::string suffix = AZStd::string::format("_SplitScreen_%s_%zu", generation.c_str(), index);
            const ViewportRect rect = GetViewportRect(index, players.size());
            const uint32_t width = AZStd::max(
                1u, aznumeric_cast<uint32_t>(m_renderSize.m_width * (rect.m_maxX - rect.m_minX)));
            const uint32_t height = AZStd::max(
                1u, aznumeric_cast<uint32_t>(m_renderSize.m_height * (rect.m_maxY - rect.m_minY)));

            AZ::RPI::AttachmentImageAssetCreator imageCreator;
            imageCreator.Begin(AZ::Data::AssetId(AZ::Uuid::CreateRandom()));
            imageCreator.SetPoolAsset(m_renderTargetPoolAsset);
            imageCreator.SetName(AZ::Name(AZStd::string::format("SplitScreenPlayer%s", suffix.c_str())), false);
            imageCreator.SetImageDescriptor(AZ::RHI::ImageDescriptor::Create2D(
                AZ::RHI::ImageBindFlags::Color | AZ::RHI::ImageBindFlags::ShaderRead,
                width,
                height,
                AZ::RHI::Format::R8G8B8A8_UNORM));
            if (!imageCreator.End(m_playerImageAssets[index]))
            {
                AZ_Error("SplitScreen", false, "Failed to create render target for local player %zu.", index);
                RemoveSplitPipelines();
                return;
            }
            m_playerImages[index] = AZ::RPI::AttachmentImage::FindOrCreate(m_playerImageAssets[index]);

            // A distinct transparent attachment for each player's real LyShine
            // canvas. The main LyShine pass renders RTT once, before compositing.
            AZ::RPI::AttachmentImageAssetCreator hud;
            hud.Begin(AZ::Data::AssetId(AZ::Uuid::CreateRandom()));
            hud.SetPoolAsset(m_renderTargetPoolAsset);
            hud.SetName(AZ::Name(AZStd::string::format("SplitScreenHud%s", suffix.c_str())), false);
            hud.SetImageDescriptor(AZ::RHI::ImageDescriptor::Create2D(
                AZ::RHI::ImageBindFlags::Color | AZ::RHI::ImageBindFlags::ShaderRead,
                width, height, AZ::RHI::Format::R8G8B8A8_UNORM));
            if (!hud.End(m_hudImageAssets[index])) { RemoveSplitPipelines(); return; }
            m_hudImages[index] = AZ::RPI::AttachmentImage::FindOrCreate(m_hudImageAssets[index]);
            if (players[index].m_canvas.IsValid())
            {
                const auto canvas = players[index].m_canvas;
                UiCanvasBus::Event(canvas, &UiCanvasInterface::SetAttachmentImageAsset, m_hudImageAssets[index]);
                UiCanvasBus::Event(canvas, &UiCanvasInterface::SetIsRenderToTexture, true);
                UiCanvasBus::Event(canvas, &UiCanvasInterface::SetTargetCanvasSize, true, AZ::Vector2(float(width), float(height)));
                // Elements may already have cached rectangles from the window
                // size before this canvas became RTT. Invalidate the entire
                // hierarchy when binding/replacing its target, including resize.
                AZStd::vector<AZ::EntityId> roots;
                UiCanvasBus::EventResult(roots, canvas, &UiCanvasInterface::GetChildElementEntityIds);
                for (AZ::EntityId root : roots)
                {
                    UiTransformBus::Event(root, &UiTransformInterface::SetRecomputeFlags,
                        UiTransformInterface::Recompute::RectAndTransform);
                }
                UiCanvasBus::Event(canvas, &UiCanvasInterface::RecomputeChangedLayouts);
            }

            AZ::RPI::RenderPipelineDescriptor descriptor = baseDescriptor;
            descriptor.m_name += suffix;
            descriptor.m_allowSubpassMerging = false;

            // LyShine uses a single pass request handler per scene. Omitting
            // UIPass before scene registration prevents LyShine from injecting
            // a second handler into every offscreen camera pipeline.
            const AZ::Name cameraTemplateName(baseDescriptor.m_rootPassTemplate + "_SplitScreenCamera");
            if (!passSystem->HasTemplate(cameraTemplateName))
            {
                auto sourceTemplate = passSystem->GetPassTemplate(AZ::Name(descriptor.m_rootPassTemplate));
                if (!sourceTemplate) { RemoveSplitPipelines(); m_rebuildPending = true; return; }
                auto cameraTemplate = sourceTemplate->Clone();
                cameraTemplate->m_name = cameraTemplateName;
                auto& requests = cameraTemplate->m_passRequests;
                requests.erase(AZStd::remove_if(requests.begin(), requests.end(), [](const AZ::RPI::PassRequest& request)
                { return request.m_passName == AZ::Name("UIPass"); }), requests.end());
                for (auto& request : requests)
                {
                    for (auto& connection : request.m_connections)
                    {
                        if (connection.m_attachmentRef.m_pass == AZ::Name("UIPass"))
                        {
                            connection.m_attachmentRef.m_pass = AZ::Name("PostProcessPass");
                            connection.m_attachmentRef.m_attachment = AZ::Name("Output");
                        }
                    }
                }
                passSystem->AddPassTemplate(cameraTemplateName, cameraTemplate);
            }
            descriptor.m_rootPassTemplate = cameraTemplateName.GetStringView();

            AZ::RPI::RenderPipelinePtr pipeline = AZ::RPI::RenderPipeline::CreateRenderPipelineForImage(
                descriptor, m_playerImageAssets[index]);
            scene->AddRenderPipeline(pipeline);
            pipeline->SetDefaultViewFromEntity(players[index].m_entityId);
            m_splitPipelines.push_back(AZStd::move(pipeline));
        }

        AZ::RPI::PassRequest compositeRequest;
        compositeRequest.m_passName = AZ::Name(AZStd::string::format("SplitScreenCompositePass_%s", generation.c_str()));
        compositeRequest.m_templateName = AZ::Name("SplitScreenCompositeTemplate");
        AZ::RPI::PassConnection outputConnection;
        outputConnection.m_localSlot = AZ::Name("InputOutput");
        outputConnection.m_attachmentRef.m_pass = AZ::Name("LyShinePass");
        outputConnection.m_attachmentRef.m_attachment = AZ::Name("ColorInputOutput");
        compositeRequest.m_connections.push_back(outputConnection);

        AZ::RPI::Ptr<AZ::RPI::Pass> compositeBasePass = passSystem->CreatePassFromRequest(&compositeRequest);
        m_compositePass = azrtti_cast<SplitScreenCompositePass*>(compositeBasePass.get());
        if (!m_compositePass || !m_originalPipeline->AddPassAfter(m_compositePass, AZ::Name("LyShinePass")))
        {
            AZ_Error("SplitScreen", false, "Failed to add the split screen compositor to the main render pipeline.");
            RemoveSplitPipelines();
            return;
        }
        uint32_t hudMask = 0;
        for (size_t i = 0; i < players.size(); ++i) if (players[i].m_canvas.IsValid()) hudMask |= 1u << i;
        m_compositePass->SetHudImages(m_hudImages, hudMask);
        m_compositePass->SetPlayerImages(m_playerImages, aznumeric_cast<uint32_t>(players.size()));
        AZ_Printf("SplitScreen", "Ready: %zu cameras, independent LyShine RTT canvases, %ux%u\n", players.size(), m_renderSize.m_width, m_renderSize.m_height);
        for (size_t index = 0; index < players.size(); ++index)
        {
            AZStd::string name;
            AzFramework::GameEntityContextRequestBus::BroadcastResult(name,
                &AzFramework::GameEntityContextRequests::GetEntityName, players[index].m_entityId);
            AZ_Printf("SplitScreen", "Viewport %zu: slot=%u camera=%s canvas=%s\n", index + 1,
                players[index].m_slot, name.c_str(), players[index].m_canvas.ToString().c_str());
        }
    }

    void SplitScreenFeatureProcessor::UpdateCameraProjections()
    {
        const AZStd::vector<Player> players = GetActivePlayers();
        if (players.size() < 2 || m_splitPipelines.size() != players.size())
        {
            return;
        }

        AZ::RPI::ViewportContextPtr viewportContext = AZ::RPI::GetDefaultViewportContext();
        if (!viewportContext)
        {
            return;
        }
        const AzFramework::WindowSize size = viewportContext->GetViewportSize();

        for (size_t index = 0; index < players.size(); ++index)
        {
            const ViewportRect rect = GetViewportRect(index, players.size());
            const float viewportWidth = size.m_width * (rect.m_maxX - rect.m_minX);
            const float viewportHeight = size.m_height * (rect.m_maxY - rect.m_minY);
            if (viewportHeight <= 0.0f)
            {
                continue;
            }

            float fov = 0.0f;
            float nearClip = 0.1f;
            float farClip = 1000.0f;
            Camera::CameraRequestBus::EventResult(fov, players[index].m_entityId, &Camera::CameraRequestBus::Events::GetFovRadians);
            Camera::CameraRequestBus::EventResult(nearClip, players[index].m_entityId, &Camera::CameraRequestBus::Events::GetNearClipDistance);
            Camera::CameraRequestBus::EventResult(farClip, players[index].m_entityId, &Camera::CameraRequestBus::Events::GetFarClipDistance);

            AZ::RPI::ViewPtr view;
            AZ::RPI::ViewProviderBus::EventResult(view, players[index].m_entityId, &AZ::RPI::ViewProvider::GetView);
            if (view)
            {
                AZ::Matrix4x4 projection;
                AZ::MakePerspectiveFovMatrixRH(projection, fov, viewportWidth / viewportHeight, nearClip, farClip, true);
                view->SetViewToClipMatrix(projection);
            }
        }
    }
}
