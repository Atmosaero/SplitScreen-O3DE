/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "SplitScreenSystemComponent.h"

#include <SplitScreen/SplitScreenTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Script/ScriptContextAttributes.h>
#include <AzFramework/Components/CameraBus.h>
#include <Atom/RPI.Public/Scene.h>
#include <LyShine/Bus/UiCanvasBus.h>

#include <Atom/RPI.Public/FeatureProcessorFactory.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>

#include <Render/SplitScreenFeatureProcessor.h>

namespace SplitScreen
{
    namespace
    {
        SplitScreenFeatureProcessorInterface* FindProcessor(AZ::EntityId cameraId, bool create = false)
        {
            if (!cameraId.IsValid()) return nullptr;
            auto* scene = AZ::RPI::Scene::GetSceneForEntityId(cameraId);
            if (!scene) return nullptr;
            auto* processor = scene->GetFeatureProcessor<SplitScreenFeatureProcessorInterface>();
            if (!processor && create)
            {
                processor = azrtti_cast<SplitScreenFeatureProcessorInterface*>(
                    scene->EnableFeatureProcessor(AZ::Name("SplitScreenFeatureProcessor")));
            }
            return processor;
        }
    }

    AZ_COMPONENT_IMPL(SplitScreenSystemComponent, "SplitScreenSystemComponent",
        SplitScreenSystemComponentTypeId);

    void SplitScreenSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SplitScreenSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }

        SplitScreenFeatureProcessor::Reflect(context);

        if (auto* behavior = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behavior->EBus<SplitScreenRequestBus>("SplitScreenRequestBus")
                ->Attribute(AZ::Script::Attributes::Category, "SplitScreen")
                ->Attribute(AZ::Script::Attributes::Module, "splitscreen")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Event("RegisterLocalPlayer", &SplitScreenRequests::RegisterLocalPlayer)
                ->Event("SetLocalPlayerCanvas", &SplitScreenRequests::SetLocalPlayerCanvas)
                ->Event("SetLocalPlayerActivated", &SplitScreenRequests::SetLocalPlayerActivated)
                ->Event("UnregisterLocalPlayer", &SplitScreenRequests::UnregisterLocalPlayer);
        }
    }

    bool SplitScreenSystemComponent::RegisterLocalPlayer(AZ::EntityId cameraId, bool activated)
    {
        if (!Camera::CameraRequestBus::HasHandlers(cameraId)) return false;
        auto* processor = FindProcessor(cameraId, true);
        return processor && processor->RegisterLocalPlayer(cameraId, activated);
    }

    bool SplitScreenSystemComponent::SetLocalPlayerCanvas(AZ::EntityId cameraId, AZ::EntityId canvasId, AZ::u32 slot)
    {
        if (canvasId.IsValid() && !UiCanvasBus::HasHandlers(canvasId)) return false;
        auto* processor = FindProcessor(cameraId);
        return processor && processor->SetLocalPlayerCanvas(cameraId, canvasId, slot);
    }

    bool SplitScreenSystemComponent::SetLocalPlayerActivated(AZ::EntityId cameraId, bool activated)
    {
        auto* processor = FindProcessor(cameraId);
        return processor && processor->SetLocalPlayerActivated(cameraId, activated);
    }

    bool SplitScreenSystemComponent::UnregisterLocalPlayer(AZ::EntityId cameraId)
    {
        auto* processor = FindProcessor(cameraId);
        return processor && processor->UnregisterLocalPlayer(cameraId);
    }

    void SplitScreenSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("SplitScreenSystemService"));
    }

    void SplitScreenSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("SplitScreenSystemService"));
    }

    void SplitScreenSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("RPISystem"));
    }

    void SplitScreenSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    SplitScreenSystemComponent::SplitScreenSystemComponent()
    {
        if (SplitScreenInterface::Get() == nullptr)
        {
            SplitScreenInterface::Register(this);
        }
    }

    SplitScreenSystemComponent::~SplitScreenSystemComponent()
    {
        if (SplitScreenInterface::Get() == this)
        {
            SplitScreenInterface::Unregister(this);
        }
    }

    void SplitScreenSystemComponent::Init()
    {
    }

    void SplitScreenSystemComponent::Activate()
    {
        SplitScreenRequestBus::Handler::BusConnect();

        AZ::RPI::PassSystemInterface* passSystem = AZ::RPI::PassSystemInterface::Get();
        AZ_Assert(passSystem, "Cannot get the pass system.");
        m_loadTemplatesHandler = AZ::RPI::PassSystemInterface::OnReadyLoadTemplatesEvent::Handler(
            [this]()
            {
                LoadPassTemplateMappings();
            });
        passSystem->ConnectEvent(m_loadTemplatesHandler);

        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessorWithInterface<SplitScreenFeatureProcessor, SplitScreenFeatureProcessorInterface>();
    }

    void SplitScreenSystemComponent::Deactivate()
    {
        m_loadTemplatesHandler.Disconnect();
        AZ::RPI::FeatureProcessorFactory::Get()->UnregisterFeatureProcessor<SplitScreenFeatureProcessor>();

        SplitScreenRequestBus::Handler::BusDisconnect();
    }

    void SplitScreenSystemComponent::LoadPassTemplateMappings()
    {
        AZ::RPI::PassSystemInterface::Get()->LoadPassTemplateMappings("Passes/SplitScreen/PassTemplates.azasset");
    }

} // namespace SplitScreen
