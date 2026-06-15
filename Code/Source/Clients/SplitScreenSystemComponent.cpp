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

#include <Atom/RPI.Public/FeatureProcessorFactory.h>
#include <Atom/RPI.Public/Pass/PassSystemInterface.h>

#include <Render/SplitScreenFeatureProcessor.h>

namespace SplitScreen
{
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

        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessor<SplitScreenFeatureProcessor>();
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
