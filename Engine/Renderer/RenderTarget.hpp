#pragma once
#include "Renderer/Utility.hpp"

class RenderTarget
{
    public:
        void Create(const glm::uvec2& size);
        void Resize(const glm::uvec2& size);
        void Destroy();

        const Image& GetImage() const;

        void TransitionLayout(ImageLayout newLayout);

    private:
        ImageLayout mLayout = ImageLayout::None;
        Image mImage;
};