#pragma once

#include <Atom/RPI.Public/Image/AttachmentImage.h>
#include <Atom/RPI.Public/Pass/FullscreenTrianglePass.h>
#include <AzCore/std/containers/array.h>

namespace SplitScreen
{
    class SplitScreenCompositePass final
        : public AZ::RPI::FullscreenTrianglePass
    {
        AZ_RPI_PASS(SplitScreenCompositePass);

    public:
        AZ_RTTI(SplitScreenCompositePass, "{3003E57F-9D38-413B-AC52-E108D3AE9661}", AZ::RPI::FullscreenTrianglePass);
        AZ_CLASS_ALLOCATOR(SplitScreenCompositePass, AZ::SystemAllocator);

        static AZ::RPI::Ptr<SplitScreenCompositePass> Create(const AZ::RPI::PassDescriptor& descriptor);
        void SetHudImages(const AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, 4>& images, uint32_t mask) { m_huds = images; m_hudMask = mask; }
        void SetPlayerImages(
            const AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, 4>& images,
            uint32_t playerCount);

    private:
        explicit SplitScreenCompositePass(const AZ::RPI::PassDescriptor& descriptor);
        void BuildInternal() override;
        void FrameBeginInternal(FramePrepareParams params) override;

        AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, 4> m_images;
        AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, 4> m_huds;
        uint32_t m_playerCount = 0;
        uint32_t m_hudMask = 0;
    };
}
