#pragma once
#include "Renderer/ComputePipeline.hpp"
#include "Renderer/GraphicsPipeline.hpp"

void CmdBindDescriptors(const CommandBuffer &commandBuffer, const GraphicsPipeline &pipeline, std::initializer_list<VkDescriptorSet> sets);
void CmdBindDescriptors(const CommandBuffer &commandBuffer, const ComputePipeline &pipeline, std::initializer_list<VkDescriptorSet> sets);

void CmdBindPipeline(const CommandBuffer& commandBuffer, const GraphicsPipeline& pipeline);
void CmdBindPipeline(const CommandBuffer& commandBuffer, const ComputePipeline& pipeline);