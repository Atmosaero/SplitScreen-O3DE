#include <Render/SplitScreenViewportPass.h>

#include <AzCore/Math/MathUtils.h>

namespace SplitScreen
{
    AZ::RPI::Ptr<SplitScreenViewportPass> SplitScreenViewportPass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew SplitScreenViewportPass(descriptor);
    }

    SplitScreenViewportPass::SplitScreenViewportPass(const AZ::RPI::PassDescriptor& descriptor)
        : ParentPass(descriptor)
    {
    }

    void SplitScreenViewportPass::SetNormalizedViewport(float minX, float minY, float maxX, float maxY)
    {
        m_minX = AZ::GetClamp(minX, 0.0f, 1.0f);
        m_minY = AZ::GetClamp(minY, 0.0f, 1.0f);
        m_maxX = AZ::GetClamp(maxX, m_minX, 1.0f);
        m_maxY = AZ::GetClamp(maxY, m_minY, 1.0f);
    }

    void SplitScreenViewportPass::FrameBeginInternal(FramePrepareParams params)
    {
        const AZ::RHI::Viewport fullViewport = params.m_viewportState;
        const float width = fullViewport.GetWidth();
        const float height = fullViewport.GetHeight();

        params.m_viewportState = AZ::RHI::Viewport(
            fullViewport.m_minX + width * m_minX,
            fullViewport.m_minX + width * m_maxX,
            fullViewport.m_minY + height * m_minY,
            fullViewport.m_minY + height * m_maxY,
            fullViewport.m_minZ,
            fullViewport.m_maxZ);

        const AZ::RHI::Scissor fullScissor = params.m_scissorState;
        const int32_t scissorWidth = fullScissor.m_maxX - fullScissor.m_minX;
        const int32_t scissorHeight = fullScissor.m_maxY - fullScissor.m_minY;
        params.m_scissorState = AZ::RHI::Scissor(
            fullScissor.m_minX + aznumeric_cast<int32_t>(scissorWidth * m_minX),
            fullScissor.m_minY + aznumeric_cast<int32_t>(scissorHeight * m_minY),
            fullScissor.m_minX + aznumeric_cast<int32_t>(scissorWidth * m_maxX),
            fullScissor.m_minY + aznumeric_cast<int32_t>(scissorHeight * m_maxY));

        ParentPass::FrameBeginInternal(params);
    }
}
