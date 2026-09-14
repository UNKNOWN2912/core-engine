#pragma once
#include "Renderer/ComputePipeline.hpp"
#include "Renderer/GraphicsPipeline.hpp"

void CmdBindDescriptors(const CommandBuffer &commandBuffer, const GraphicsPipeline &pipeline, std::initializer_list<VkDescriptorSet> sets);
void CmdBindDescriptors(const CommandBuffer &commandBuffer, const ComputePipeline &pipeline, std::initializer_list<VkDescriptorSet> sets);
void CmdBindDescriptors(const CommandBuffer &commandBuffer, const GraphicsPipeline &pipeline, std::initializer_list<const Descriptor *> descriptors);
void CmdBindVertexBuffers(const CommandBuffer &commandBuffer, const std::vector<Buffer> &buffers);
void CmdTransitionImageLayout(const CommandBuffer &commandBuffer, ImageLayout oldLayout, ImageLayout newLayout, ImageAspect aspectMask, const ImageDeprecated &image);
void CmdTransferImageData(const CommandBuffer &commandBuffer, const Buffer &srcBuffer, ImageDeprecated &dstImage, ImageAspect aspectMask, uint32_t layerIndex = 0, uint32_t miplevelIndex = 0);