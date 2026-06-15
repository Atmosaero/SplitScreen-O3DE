
#include <SplitScreen/SplitScreenTypeIds.h>
#include <SplitScreenModuleInterface.h>
#include "SplitScreenSystemComponent.h"

#include <AzCore/RTTI/RTTI.h>

#include <Components/SplitScreenComponent.h>

namespace SplitScreen
{
    class SplitScreenModule
        : public SplitScreenModuleInterface
    {
    public:
        AZ_RTTI(SplitScreenModule, SplitScreenModuleTypeId, SplitScreenModuleInterface);
        AZ_CLASS_ALLOCATOR(SplitScreenModule, AZ::SystemAllocator);

        SplitScreenModule()
        {
            m_descriptors.insert(m_descriptors.end(),
                {
                    SplitScreenSystemComponent::CreateDescriptor(),
                    SplitScreenComponent::CreateDescriptor(),
                });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const
        {
            return AZ::ComponentTypeList{ azrtti_typeid<SplitScreenSystemComponent>() };
        }
    };
}// namespace SplitScreen

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), SplitScreen::SplitScreenModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_SplitScreen, SplitScreen::SplitScreenModule)
#endif
