/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Components/SplitScreenComponentController.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <Atom/RPI.Public/Scene.h>
#include <Render/SplitScreenFeatureProcessor.h>

namespace SplitScreen
{
    void SplitScreenComponentConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SplitScreenComponentConfig>()
                ->Version(1)
                ->Field("Activated", &SplitScreenComponentConfig::m_activated)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<SplitScreenComponentConfig>("Player Viewport configuration", "")
                    ->DataElement(AZ::Edit::UIHandlers::CheckBox, &SplitScreenComponentConfig::m_activated,
                        "Activated", "Include this camera in the local split screen layout");
            }
        }
    }

    void SplitScreenComponentController::Reflect(AZ::ReflectContext* context)
    {
        SplitScreenComponentConfig::Reflect(context);

        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SplitScreenComponentController>()
                ->Version(0)
                ->Field("Configuration", &SplitScreenComponentController::m_configuration);

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<SplitScreenComponentController>(
                    "SplitScreenComponentController", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &SplitScreenComponentController::m_configuration, "Configuration", "")
                        ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                    ;
            }
        }
    }

    void SplitScreenComponentController::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("CameraService"));
    }

    void SplitScreenComponentController::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("LocalPlayerService"));
    }

    void SplitScreenComponentController::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("LocalPlayerService"));
    }

    void SplitScreenComponentController::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("CameraService"));
    }

    SplitScreenComponentController::SplitScreenComponentController(const SplitScreenComponentConfig& config)
        : m_configuration(config)
    {
    }

    void SplitScreenComponentController::Activate(AZ::EntityId entityId)
    {
        m_entityId = entityId;

        if (AZ::RPI::Scene* scene = AZ::RPI::Scene::GetSceneForEntityId(entityId))
        {
            m_featureProcessor = scene->GetFeatureProcessor<SplitScreenFeatureProcessorInterface>();
            if (!m_featureProcessor)
            {
                m_featureProcessor = azrtti_cast<SplitScreenFeatureProcessorInterface*>(
                    scene->EnableFeatureProcessor(AZ::Name("SplitScreenFeatureProcessor")));
            }
        }
        if (m_featureProcessor)
        {
            m_featureProcessor->RegisterLocalPlayer(m_entityId, m_configuration.m_activated);
        }
    }

    void SplitScreenComponentController::Deactivate()
    {
        if (m_featureProcessor)
        {
            m_featureProcessor->UnregisterLocalPlayer(m_entityId);
            m_featureProcessor = nullptr;
        }
    }

    void SplitScreenComponentController::SetConfiguration(const SplitScreenComponentConfig& config)
    {
        m_configuration = config;
        if (m_featureProcessor)
        {
            m_featureProcessor->SetLocalPlayerActivated(m_entityId, m_configuration.m_activated);
        }
    }

    const SplitScreenComponentConfig& SplitScreenComponentController::GetConfiguration() const
    {
        return m_configuration;
    }

}
