#pragma once
#include "Renderer/Utility.hpp"

class RenderTarget
{
  public:
    RenderTarget() = default;
    RenderTarget(const RenderTarget &) = default;
    RenderTarget(RenderTarget &&) = delete;
    RenderTarget &operator=(const RenderTarget &) = default;
    RenderTarget &operator=(RenderTarget &&renderTarget) noexcept
    {
        mImage = renderTarget.mImage;
        mLayout = renderTarget.mLayout;

        renderTarget.mImage = Image();
        renderTarget.mLayout = ImageLayout::None;

        return *this;
    }
    RenderTarget(const glm::uvec2 &size);
    ~RenderTarget();

    void Resize(const glm::uvec2 &size);
    void Destroy();

    [[nodiscard]] const Image &GetImage() const;

    void TransitionLayout(ImageLayout newLayout);

  private:
    ImageLayout mLayout = ImageLayout::None;
    Image mImage;
};