
#pragma once

#include <SplitScreen/SplitScreenTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>
#include <AzCore/Component/EntityId.h>

namespace SplitScreen
{
    class SplitScreenRequests
    {
    public:
        AZ_RTTI(SplitScreenRequests, SplitScreenRequestsTypeId);
        virtual ~SplitScreenRequests() = default;
        // Return false when the camera/scene is not ready or the request is invalid.
        virtual bool RegisterLocalPlayer(AZ::EntityId cameraId, bool activated) = 0;
        virtual bool SetLocalPlayerCanvas(AZ::EntityId cameraId, AZ::EntityId canvasId, AZ::u32 slot) = 0;
        virtual bool SetLocalPlayerActivated(AZ::EntityId cameraId, bool activated) = 0;
        virtual bool UnregisterLocalPlayer(AZ::EntityId cameraId) = 0;
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
