/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Components/SplitScreenComponentController.h>
#include <AzFramework/Components/ComponentAdapter.h>

namespace SplitScreen
{
    inline constexpr AZ::TypeId SplitScreenComponentTypeId { "{942442D9-7A4E-4ABA-8C37-DB652A1DE72F}" };

    class SplitScreenComponent final
        : public AzFramework::Components::ComponentAdapter<SplitScreenComponentController, SplitScreenComponentConfig>
    {
    public:
        using BaseClass = AzFramework::Components::ComponentAdapter<SplitScreenComponentController, SplitScreenComponentConfig>;
        AZ_COMPONENT(SplitScreenComponent, SplitScreenComponentTypeId, BaseClass);

        SplitScreenComponent() = default;
        SplitScreenComponent(const SplitScreenComponentConfig& config);

        static void Reflect(AZ::ReflectContext* context);
    };
}
