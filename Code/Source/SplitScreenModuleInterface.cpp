
#include "SplitScreenModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <SplitScreen/SplitScreenTypeIds.h>

#include <Clients/SplitScreenSystemComponent.h>

namespace SplitScreen
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(SplitScreenModuleInterface,
        "SplitScreenModuleInterface", SplitScreenModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(SplitScreenModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(SplitScreenModuleInterface, AZ::SystemAllocator);

    SplitScreenModuleInterface::SplitScreenModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            SplitScreenSystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList SplitScreenModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<SplitScreenSystemComponent>(),
        };
    }
} // namespace SplitScreen
