#pragma once

#include <Atom/RPI.Public/Pass/ParentPass.h>

namespace SplitScreen
{
    class SplitScreenViewportPass final
        : public AZ::RPI::ParentPass
    {
        AZ_RPI_PASS(SplitScreenViewportPass);

    public:
        AZ_RTTI(SplitScreenViewportPass, "{7F55E279-E3D1-466E-A5BD-52453A05E129}", AZ::RPI::ParentPass);
        AZ_CLASS_ALLOCATOR(SplitScreenViewportPass, AZ::SystemAllocator);

        static AZ::RPI::Ptr<SplitScreenViewportPass> Create(const AZ::RPI::PassDescriptor& descriptor);
        void SetNormalizedViewport(float minX, float minY, float maxX, float maxY);

    private:
        explicit SplitScreenViewportPass(const AZ::RPI::PassDescriptor& descriptor);
        void FrameBeginInternal(FramePrepareParams params) override;

        float m_minX = 0.0f;
        float m_minY = 0.0f;
        float m_maxX = 1.0f;
        float m_maxY = 1.0f;
    };
}
