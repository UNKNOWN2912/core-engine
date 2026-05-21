#include "Helper.hpp"


void CmdBindDescriptors(const CommandBuffer &commandBuffer, const ComputePipeline &pipeline, std::initializer_list<VkDescriptorSet> sets) 
{
    vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipelineLayout(), 0, sets.size(), sets.begin(), 0, 0);
}
void CmdBindDescriptors(const CommandBuffer &commandBuffer, const GraphicsPipeline &pipeline, std::initializer_list<VkDescriptorSet> sets) 
{
    vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipelineLayout(), 0, sets.size(), sets.begin(), 0, 0);
}

void CmdBindPipeline(const CommandBuffer& commandBuffer, const GraphicsPipeline& pipeline)
{
    vkCmdBindPipeline(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetHandle());
}

void CmdBindPipeline(const CommandBuffer& commandBuffer, const ComputePipeline& pipeline)
{
    vkCmdBindPipeline(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetHandle());
}