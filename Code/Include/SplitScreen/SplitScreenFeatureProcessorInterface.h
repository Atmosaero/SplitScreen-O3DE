/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/base.h>
#include <AzCore/Component/EntityId.h>
#include <Atom/RPI.Public/FeatureProcessor.h>

namespace SplitScreen
{
    // SplitScreenFeatureProcessorInterface provides an interface to the feature processor for code outside of Atom
    class SplitScreenFeatureProcessorInterface
        : public AZ::RPI::FeatureProcessor
    {
    public:
        AZ_RTTI(SplitScreenFeatureProcessorInterface, "{E18A8DBC-F116-4CFC-BAED-EA40A75F47DD}", AZ::RPI::FeatureProcessor);

        virtual bool RegisterLocalPlayer(AZ::EntityId entityId, bool activated) = 0;
        // Slot fixes controller/HUD ordering independently of entity creation order.
        // Canvas ownership stays with the caller; detach before unloading it.
        virtual bool SetLocalPlayerCanvas(AZ::EntityId camera, AZ::EntityId canvas, uint32_t slot) = 0;
        virtual bool UnregisterLocalPlayer(AZ::EntityId entityId) = 0;
        virtual bool SetLocalPlayerActivated(AZ::EntityId entityId, bool activated) = 0;

    };
}
