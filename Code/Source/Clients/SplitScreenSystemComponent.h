/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/Component.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>
#include <SplitScreen/SplitScreenBus.h>

namespace SplitScreen
{
    class SplitScreenSystemComponent
        : public AZ::Component
        , protected SplitScreenRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(SplitScreenSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        SplitScreenSystemComponent();
        ~SplitScreenSystemComponent();

        bool RegisterLocalPlayer(AZ::EntityId cameraId, bool activated) override;
        bool SetLocalPlayerCanvas(AZ::EntityId cameraId, AZ::EntityId canvasId, AZ::u32 slot) override;
        bool SetLocalPlayerActivated(AZ::EntityId cameraId, bool activated) override;
        bool UnregisterLocalPlayer(AZ::EntityId cameraId) override;

    protected:
        ////////////////////////////////////////////////////////////////////////
        // SplitScreenRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

        void LoadPassTemplateMappings();

        AZ::RPI::PassSystemInterface::OnReadyLoadTemplatesEvent::Handler m_loadTemplatesHandler;
    };

} // namespace SplitScreen
