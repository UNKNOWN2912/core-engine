#pragma once
#include "Renderer/Utility.hpp"
#include <string_view>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>


class Texture
{
    public:
        void Create(void* data, VkFormat format, const glm::uvec2& size, uint32_t binding);
        void Load(std::string_view filename, uint32_t binding);

        VkDescriptorSet GetDescriptorSet() const;
        VkDescriptorSetLayout GetSetLayout() const;

        void SetDataToDescriptorSet(VkDescriptorSet descriptorSet, uint32_t binding);

        bool IsValid() const { return mIsValid; }

      private:
        bool mIsValid = false;
        Image mImage;
        Buffer mStagingBuffer;

        VkSampler mSampler;

};