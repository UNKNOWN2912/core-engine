#pragma once
#include "Renderer/Utility.hpp"
#include <string_view>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>


class Texture
{
    public:
        void Create(void* data, const glm::uvec2& size, ImageFormat format);
        void Destroy();

        void Load(std::string_view filename);
        const Image& GetImage() const { return mImage; }
        bool IsValid() const { return mIsValid; }

        ~Texture();
      private:
        bool mIsValid = false;
        Image mImage;
        Buffer mStagingBuffer;
};