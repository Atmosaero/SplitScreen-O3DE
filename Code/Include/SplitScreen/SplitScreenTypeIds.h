
#pragma once

namespace SplitScreen
{
    // System Component TypeIds
    inline constexpr const char* SplitScreenSystemComponentTypeId = "{3FA7D533-19AC-4600-8EC8-57879108D774}";
    inline constexpr const char* SplitScreenEditorSystemComponentTypeId = "{8AE4AAF1-A8B1-4921-9ACF-2FD5A5310EC3}";

    // Module derived classes TypeIds
    inline constexpr const char* SplitScreenModuleInterfaceTypeId = "{35D4A510-BB88-46ED-926A-47CD3A48D5A3}";
    inline constexpr const char* SplitScreenModuleTypeId = "{28CF7F92-AACB-41B1-91AC-D6D720D42940}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* SplitScreenEditorModuleTypeId = SplitScreenModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* SplitScreenRequestsTypeId = "{5244239E-6185-46C2-8F22-0A5024EA3069}";
} // namespace SplitScreen
