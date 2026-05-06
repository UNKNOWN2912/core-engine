#pragma once
#include "imgui.h"
#include <glm/glm.hpp>
#include <string_view>

namespace ImGuiHelper
{
    inline ImFont* iconFont;

    void DragVec3(std::string_view label, glm::vec3& value, float speed = 1.f);
    bool FileDialog(const std::string& label, std::string& filename, bool& opened);

    void ClearFileDialogData(std::string_view label);

    void IconCharacter(char ch);

    void IconCharacterSameLine(char ch);
    
}