#include "ImGuiHelper.hpp"
#include "imgui.h"



void ImGuiHelper::DragVec3(std::string_view label, glm::vec3& value, float speed) 
{
    ImGui::DragFloat3(label.data(), &value.x, speed);    
}
