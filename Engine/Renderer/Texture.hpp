#pragma once
#include "Renderer/Utility.hpp"
#include <string_view>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>


class Texture
{
    public:
        void Create(void* data, const glm::uvec2& size, ImageFormat format);
        void Load(std::string_view filename);
        
        void UpdateDescriptorSet(VkDescriptorSet descriptorSet, uint32_t binding);
        
        void SetSampler(Filter minFilter, Filter magFilter, const std::array<AddressMode, 3>& addressMode);
        
        bool IsValid() const { return mIsValid; }

      private:
        bool mIsValid = false;
        Image mImage;
        Buffer mStagingBuffer;

        VkSampler mSampler = VK_NULL_HANDLE;
};