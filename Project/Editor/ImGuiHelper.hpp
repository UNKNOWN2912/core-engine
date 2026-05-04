#pragma once
#include <glm/glm.hpp>
#include <string_view>

namespace ImGuiHelper
{
    void DragVec3(std::string_view label, glm::vec3& value, float speed = 1.f);
}