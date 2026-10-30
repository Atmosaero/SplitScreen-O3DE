#include <Render/SplitScreenCompositePass.h>

#include <Atom/RPI.Public/Shader/ShaderResourceGroup.h>

namespace SplitScreen
{
    AZ::RPI::Ptr<SplitScreenCompositePass> SplitScreenCompositePass::Create(const AZ::RPI::PassDescriptor& descriptor)
    {
        return aznew SplitScreenCompositePass(descriptor);
    }

    SplitScreenCompositePass::SplitScreenCompositePass(const AZ::RPI::PassDescriptor& descriptor)
        : FullscreenTrianglePass(descriptor)
    {
    }

    void SplitScreenCompositePass::SetPlayerImages(
        const AZStd::array<AZ::Data::Instance<AZ::RPI::AttachmentImage>, 4>& images,
        uint32_t playerCount)
    {
        m_images = images;
        m_playerCount = playerCount;
        QueueForBuildAndInitialization();
    }

    void SplitScreenCompositePass::BuildInternal()
    {
        static const AZ::Name slotNames[] = {
            AZ::Name("Player0"), AZ::Name("Player1"), AZ::Name("Player2"), AZ::Name("Player3")
        };

        for (uint32_t index = 0; index < m_images.size(); ++index)
        {
            if (m_playerCount > 0)
            {
                const uint32_t imageIndex = AZStd::min(index, m_playerCount - 1);
                if (m_images[imageIndex])
                {
                    AttachImageToSlot(slotNames[index], m_images[imageIndex]);
                }
            }
        }
        for (uint32_t index = 0; index < 4; ++index)
        {
            if (m_playerCount == 0) break;
            const uint32_t source = AZStd::min(index, m_playerCount - 1);
            if (m_huds[source]) AttachImageToSlot(AZ::Name(AZStd::string::format("Hud%u", index)), m_huds[source]);
        }
        FullscreenTrianglePass::BuildInternal();
    }

    void SplitScreenCompositePass::FrameBeginInternal(FramePrepareParams params)
    {
        FullscreenTrianglePass::FrameBeginInternal(params);
        if (m_shaderResourceGroup)
        {
            const AZ::RHI::ShaderInputConstantIndex countIndex =
                m_shaderResourceGroup->FindShaderInputConstantIndex(AZ::Name("m_playerCount"));
            if (!countIndex.IsNull())
            {
                m_shaderResourceGroup->SetConstant(countIndex, m_playerCount);
            }
            const auto hudIndex = m_shaderResourceGroup->FindShaderInputConstantIndex(AZ::Name("m_hudMask"));
            if (!hudIndex.IsNull()) m_shaderResourceGroup->SetConstant(hudIndex, m_hudMask);
        }
    }
}
