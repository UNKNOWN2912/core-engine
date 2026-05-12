#include "RendererAttachments.hpp"


void DeferredAttachment::CreateAttachment(const glm::uvec2& size) 
{
    albedo = CreateImage(size, ImageFormat::RGBA8, ImageUsage::Color | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    position = CreateImage(size, ImageFormat::RGBA32, ImageUsage::Color | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    normal = CreateImage(size, ImageFormat::RGBA32, ImageUsage::Color | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    depth = CreateImage(size, ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageAspect::Depth, MemoryProperty::DeviceLocal);

    this->size = size;
}

void DeferredAttachment::ResizeAttachment(const glm::uvec2& size) 
{
    if(size == this->size)
        return;
    
    DestroyAttachment();
    CreateAttachment(size);
}

void DeferredAttachment::DestroyAttachment() 
{
    DestroyImage(albedo);    
    DestroyImage(position);    
    DestroyImage(normal);    
    DestroyImage(depth);    
}
