/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/Component.h>

#include <SplitScreen/SplitScreenFeatureProcessorInterface.h>

namespace SplitScreen
{
    class SplitScreenComponentConfig final
        : public AZ::ComponentConfig
    {
    public:
        AZ_RTTI(SplitScreenComponentConfig, "{32F4C6F1-CF25-4059-8E78-496EE4F1A581}", ComponentConfig);
        AZ_CLASS_ALLOCATOR(SplitScreenComponentConfig, AZ::SystemAllocator);
        static void Reflect(AZ::ReflectContext* context);

        SplitScreenComponentConfig() = default;

        bool m_activated = true;
    };

    class SplitScreenComponentController final
    {
    public:
        friend class EditorSplitScreenComponent;

        AZ_RTTI(SplitScreenComponentController, "{8ECC7AD0-E7EA-48A4-A1B0-516DC3FFAC98}");
        AZ_CLASS_ALLOCATOR(SplitScreenComponentController, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        SplitScreenComponentController() = default;
        SplitScreenComponentController(const SplitScreenComponentConfig& config);

        void Activate(AZ::EntityId entityId);
        void Deactivate();
        void SetConfiguration(const SplitScreenComponentConfig& config);
        const SplitScreenComponentConfig& GetConfiguration() const;

    private:

        AZ_DISABLE_COPY(SplitScreenComponentController);

        SplitScreenFeatureProcessorInterface* m_featureProcessor = nullptr;
        AZ::EntityId m_entityId;
        
        SplitScreenComponentConfig m_configuration;

    };
}
