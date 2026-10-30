/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <SplitScreen/SplitScreenFeatureProcessorInterface.h>
#include <AzCore/Component/TickBus.h>
#include <Atom/RPI.Public/RenderPipeline.h>
#include <Atom/RPI.Public/Image/AttachmentImage.h>
#include <Atom/RPI.Reflect/Image/AttachmentImageAsset.h>
#include <Atom/RPI.Reflect/ResourcePoolAsset.h>
#include <AzCore/std/containers/array.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzCore/std/containers/vector.h>

namespace SplitScreen
{
    class SplitScreenCompositePass;

    class SplitScreenFeatureProcessor final
        : public SplitScreenFeatureProcessorInterface
        , private AZ::TickBus::Handler
    {
    public:
        AZ_RTTI(SplitScreenFeatureProcessor, "{0B693683-53DC-4F75-93E2-4BB7C1B69A8B}", SplitScreenFeatureProcessorInterface);
        AZ_CLASS_ALLOCATOR(SplitScreenFeatureProcessor, AZ::SystemAllocator)

        static void Reflect(AZ::ReflectContext* context);

        SplitScreenFeatureProcessor() = default;
        virtual ~SplitScreenFeatureProcessor() = default;

        // FeatureProcessor overrides
        void Activate() override;
        void Deactivate() override;
        void Simulate(const FeatureProcessor::SimulatePacket& packet) override;

        bool RegisterLocalPlayer(AZ::EntityId entityId, bool activated) override;
        bool SetLocalPlayerCanvas(AZ::EntityId camera, AZ::EntityId canvas, uint32_t slot) override;
        bool UnregisterLocalPlayer(AZ::EntityId entityId) override;
        bool SetLocalPlayerActivated(AZ::EntityId entityId, bool activated) override;

    private:
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        struct Player
        {
            AZ::EntityId m_entityId;
            bool m_activated = true;
            AZ::EntityId m_canvas;
            uint32_t m_slot = 4;
            bool m_wasRtt = false;
            AZ::Data::Asset<AZ::RPI::AttachmentImageAsset> m_previousImage;
        };

        struct ViewportRect
        {
            float m_minX;
            float m_minY;
            float m_maxX;
            float m_maxY;
        };

        void RebuildPipelines();
        void RemoveSplitPipelines();
        void UpdateCameraProjections();
        AZStd::vector<Player> GetActivePlayers() const;
        static ViewportRect GetViewportRect(size_t index, size_t playerCount);

        AZStd::unordered_map<AZ::EntityId, Player> m_players;
        AZStd::vector<AZ::RPI::RenderPipelinePtr> m_splitPipelines;
        AZ::RPI::RenderPipelinePtr m_originalPipeline;
        AZ::RPI::Ptr<SplitScreenCompositePass> m_compositePass;
        AZStd::array<AZ::Data::Asset<AZ::RPI::AttachmentImageAsset>, 4> m_playerImageAssets;
        AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, 4> m_playerImages;
        AZStd::array<AZ::Data::Asset<AZ::RPI::AttachmentImageAsset>, 4> m_hudImageAssets;
        AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, 4> m_hudImages;
        AZ::Data::Asset<AZ::RPI::ResourcePoolAsset> m_renderTargetPoolAsset;
        AzFramework::WindowSize m_renderSize;
        bool m_rebuildPending = false;
    };
}
