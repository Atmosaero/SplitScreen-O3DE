/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Serialization/SerializeContext.h>
#include "SplitScreenEditorSystemComponent.h"

#include <SplitScreen/SplitScreenTypeIds.h>

namespace SplitScreen
{
    AZ_COMPONENT_IMPL(SplitScreenEditorSystemComponent, "SplitScreenEditorSystemComponent",
        SplitScreenEditorSystemComponentTypeId, BaseSystemComponent);

    void SplitScreenEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SplitScreenEditorSystemComponent, SplitScreenSystemComponent>()
                ->Version(0);
        }
    }

    SplitScreenEditorSystemComponent::SplitScreenEditorSystemComponent() = default;

    SplitScreenEditorSystemComponent::~SplitScreenEditorSystemComponent() = default;

    void SplitScreenEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("SplitScreenSystemEditorService"));
    }

    void SplitScreenEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("SplitScreenSystemEditorService"));
    }

    void SplitScreenEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void SplitScreenEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void SplitScreenEditorSystemComponent::Activate()
    {
        SplitScreenSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void SplitScreenEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        SplitScreenSystemComponent::Deactivate();
    }

} // namespace SplitScreen
