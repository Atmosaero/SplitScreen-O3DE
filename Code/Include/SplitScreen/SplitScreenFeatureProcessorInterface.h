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

        virtual void RegisterLocalPlayer(AZ::EntityId entityId, bool activated) = 0;
        virtual void UnregisterLocalPlayer(AZ::EntityId entityId) = 0;
        virtual void SetLocalPlayerActivated(AZ::EntityId entityId, bool activated) = 0;

    };
}
