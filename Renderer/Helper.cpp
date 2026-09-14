#include "Helper.hpp"
#include "Renderer/Converter.hpp"
#include <vector>

void CmdBindDescriptors(const CommandBuffer &commandBuffer, const ComputePipeline &pipeline, std::initializer_list<VkDescriptorSet> sets)
{
    vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipelineLayout(), 0, sets.size(), sets.begin(), 0, 0);
}
void CmdBindDescriptors(const CommandBuffer &commandBuffer, const GraphicsPipeline &pipeline, std::initializer_list<VkDescriptorSet> sets)
{
    vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipelineLayout(), 0, sets.size(), sets.begin(), 0, 0);
}

void CmdBindDescriptors(const CommandBuffer &commandBuffer, const GraphicsPipeline &pipeline, std::initializer_list<const Descriptor *> descriptors)
{
    assert(descriptors.size() <= 16);

    VkDescriptorSet sets[16];
    for (int i = 0; i < descriptors.size(); i++)
    {
        sets[i] = descriptors.begin()[i]->GetDescriptorSet();
    }

    vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipelineLayout(), 0, descriptors.size(), sets, 0, 0);
}
void CmdBindVertexBuffers(const CommandBuffer &commandBuffer, const std::vector<Buffer> &buffers)
{
    assert(buffers.size() <= 16);

    VkBuffer vkbuffers[16] = {VK_NULL_HANDLE};
    VkDeviceSize offsets[16] = {0};

    for (size_t i = 0; i < buffers.size(); i++)
    {
        vkbuffers[i] = buffers[i].handle;
    }

    vkCmdBindVertexBuffers(commandBuffer.GetHandle(), 0, buffers.size(), vkbuffers, offsets);
}

void CmdTransitionImageLayout(const CommandBuffer &commandBuffer, ImageLayout oldLayout, ImageLayout newLayout, ImageAspect aspectMask, const ImageDeprecated &image)
{
    VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
            .oldLayout = GetVulkanImageLayout(oldLayout),
            .newLayout = GetVulkanImageLayout(newLayout),
            .image = image.handle,
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(aspectMask),
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };

    vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CmdTransferImageData(const CommandBuffer &commandBuffer, const Buffer &srcBuffer, ImageDeprecated &dstImage, ImageAspect aspectMask, uint32_t layerIndex, uint32_t miplevelIndex)
{
    VkBufferImageCopy region =
        {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask = GetVulkanImageAspect(aspectMask),
                    .mipLevel = miplevelIndex,
                    .baseArrayLayer = layerIndex,
                    .layerCount = 1,
                },
            .imageOffset = {0, 0, 0},
            .imageExtent = {dstImage.size.x, dstImage.size.y, 1},
        };

    vkCmdCopyBufferToImage(commandBuffer.GetHandle(), srcBuffer.handle, dstImage.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
