#pragma once
#include <glm/glm.hpp>

struct Transform
{
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);

    glm::mat4 GetMatrix() const;

    Transform(){}
    Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale): 
        position(position), rotation(rotation), scale(scale) 
    {
        
    }
};