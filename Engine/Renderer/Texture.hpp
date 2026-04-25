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
        void Load(std::string_view filename);
        bool IsValid() const { return mIsValid; }

        const Image& GetImage() const { return mImage; }

      private:
        bool mIsValid = false;
        Image mImage;
        Buffer mStagingBuffer;
};