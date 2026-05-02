#pragma once
#include <glm/glm.hpp>
#include "Utility.hpp"

struct DeferredSubpassAttachment
{
    void CreateAttachment(const glm::uvec2& size);
    void ResizeAttachment(const glm::uvec2& size);
    void DestroyAttachment();

    Image albedo;
    Image position;
    Image normal;
    Image depth;

    glm::uvec2 size;
};