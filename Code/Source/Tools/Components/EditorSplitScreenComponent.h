/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/Utils/EditorRenderComponentAdapter.h>

#include <AzCore/Component/TickBus.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>
#include <AzToolsFramework/API/ComponentEntitySelectionBus.h>
#include <AzToolsFramework/Entity/EditorEntityInfoBus.h>
#include <AzToolsFramework/ToolsComponents/EditorComponentAdapter.h>
#include <Components/SplitScreenComponent.h>

#include <SplitScreen/SplitScreenTypeIds.h>

namespace SplitScreen
{
    inline constexpr AZ::TypeId EditorComponentTypeId { "{03BCFD47-BB30-46A1-A34D-81E406090A47}" };

    class EditorSplitScreenComponent final
        : public AZ::Render::EditorRenderComponentAdapter<SplitScreenComponentController, SplitScreenComponent, SplitScreenComponentConfig>
        , private AzToolsFramework::EditorComponentSelectionRequestsBus::Handler
        , private AzFramework::EntityDebugDisplayEventBus::Handler
        , private AZ::TickBus::Handler
        , private AzToolsFramework::EditorEntityInfoNotificationBus::Handler
    {
    public:
        using BaseClass = AZ::Render::EditorRenderComponentAdapter <SplitScreenComponentController, SplitScreenComponent, SplitScreenComponentConfig>;
        AZ_EDITOR_COMPONENT(EditorSplitScreenComponent, EditorComponentTypeId, BaseClass);

        static void Reflect(AZ::ReflectContext* context);

        EditorSplitScreenComponent();
        EditorSplitScreenComponent(const SplitScreenComponentConfig& config);

        // AZ::Component overrides
        void Activate() override;
        void Deactivate() override;

    private:

        bool ShouldActivateController() const override;

        // AZ::TickBus overrides
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;


    };
}
