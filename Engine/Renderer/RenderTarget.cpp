#include "RenderTarget.hpp"

RenderTarget::RenderTarget(const glm::uvec2 &size)
{
    mImage = CreateImage(size, ImageFormat::RGBA8UNORM, ImageUsage::Sampler | ImageUsage::Storage | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal);
}
RenderTarget::~RenderTarget()
{
    Destroy();
}
void RenderTarget::Resize(const glm::uvec2 &size)
{
    if (mImage.size == size)
    {
        return;
    }

    Destroy();
    mImage = CreateImage(size, ImageFormat::RGBA8UNORM, ImageUsage::Sampler | ImageUsage::Storage | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal);
    TransitionImageLayout(ImageLayout::None, mLayout, ImageAspect::Color, mImage);
}

void RenderTarget::Destroy()
{
    DestroyImage(mImage);
}

const Image &RenderTarget::GetImage() const
{
    return mImage;
}

void RenderTarget::TransitionLayout(ImageLayout newLayout)
{
    TransitionImageLayout(mLayout, newLayout, ImageAspect::Color, mImage);
    mLayout = newLayout;
}
