
#pragma once

#include <SplitScreen/SplitScreenTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace SplitScreen
{
    class SplitScreenRequests
    {
    public:
        AZ_RTTI(SplitScreenRequests, SplitScreenRequestsTypeId);
        virtual ~SplitScreenRequests() = default;
        // Put your public methods here
    };

    class SplitScreenBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using SplitScreenRequestBus = AZ::EBus<SplitScreenRequests, SplitScreenBusTraits>;
    using SplitScreenInterface = AZ::Interface<SplitScreenRequests>;

} // namespace SplitScreen
