#pragma once
#include <glm/glm.hpp>

struct Ray
{
    glm::vec3 origin = glm::vec3(0);
    glm::vec3 direction = glm::vec3(0);

    glm::vec3 GetPoint(float t)
    {
        return origin + (direction * t);
    }

    float AABBIntersection(const glm::vec3 &min, const glm::vec3 &max);
};

