#pragma once
#include "Renderer/Shader.hpp"
#include <cstdint>
#include <vector>

enum class DescriptorType
{
    UniformBuffer,
    StorageBuffer,
    CombinedSampler
};

struct DescriptorDescription
{
    DescriptorType type;
    uint32_t binding;
    ShaderType stage;
};


class PipelineLayout
{
    public:
        PipelineLayout& SetId(uint64_t id) { mId = id; return *this; }
        PipelineLayout& AddDescriptorInfo(DescriptorType type, uint32_t binding, ShaderType stage) { mDescriptorInfos.emplace_back(type, binding, stage); return *this; }
        
        uint64_t GetId() const { return mId; }
        const std::vector<DescriptorDescription>& GetDescriptorInfos() const { return mDescriptorInfos; }
    private:
        std::vector<DescriptorDescription> mDescriptorInfos;
        uint64_t mId = 0;
};