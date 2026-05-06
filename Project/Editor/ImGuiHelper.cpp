#include "ImGuiHelper.hpp"
#include "Core/Application.hpp"
#include "Core/Macro.hpp"
#include "imgui.h"
#include <filesystem>
#include <string>
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

namespace ImGuiHelper
{
    void DragVec3(std::string_view label, glm::vec3& value, float speed) 
    {
        ImGui::DragFloat3(label.data(), &value.x, speed);    
    }


    struct FileDialogData
    {
        std::string currentPath;
        std::string editingPath;
        std::string searchItem;
        uint32_t selected = UINT32_MAX;
        bool searchEnable = false;
    };

    std::string toLower(const std::string& string)
    {
        std::string result;
        result.resize(string.size());

        for (int i = 0; i < string.size(); i++)
        {
            result[i] = std::tolower(string[i]);
        }

        return result;

    }
    
    static std::unordered_map<std::string, FileDialogData> fileDialogDataMap;

    bool FileDialog(const std::string& label, std::string& filename, bool& opened)
    {
        if(!opened)
            return false;
    
        std::vector<std::filesystem::directory_entry> directories;


        if(!fileDialogDataMap.contains(label))
        {
            fileDialogDataMap[label].currentPath = std::filesystem::current_path();
            fileDialogDataMap[label].editingPath = std::filesystem::current_path();
        }

        FileDialogData& data = fileDialogDataMap[label]; 

        for (auto entry : std::filesystem::directory_iterator(data.currentPath))
        {
            if(!entry.is_directory())
                continue;

            std::string searchFormatted = toLower(entry.path());
            std::string searchingFormatted = toLower(data.searchItem);
            searchFormatted.erase(searchFormatted.begin(), searchFormatted.begin() + searchFormatted.find_last_of('/') + 1);

            if(data.searchItem.size() != 0)
            {
                if(!searchFormatted.contains(searchingFormatted))
                    continue;
            }
            directories.emplace_back(entry);
        }
        for (auto entry : std::filesystem::directory_iterator(data.currentPath))
        {
            if(entry.is_directory())
                continue;
            
            std::string searchFormatted = toLower(entry.path());
            std::string searchingFormatted = toLower(data.searchItem);
        
            if(data.searchItem.size() != 0)
            {
                if(!searchFormatted.contains(searchingFormatted))
                    continue;
            }

            directories.emplace_back(entry);
        }

        ImGui::Begin(label.data(), &opened);

        if(ImGui::InputText("##path", &data.editingPath, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            data.currentPath = data.editingPath; 
        }
        ImGui::SameLine();
        ImGui::PushFont(iconFont);
        if(ImGui::Button("C"))
        {
            size_t pos = data.editingPath.find_last_of('/');
            data.editingPath.erase(data.editingPath.begin() + pos, data.editingPath.end());
            data.currentPath = data.editingPath;
        }
        ImGui::SameLine();
        if(ImGui::Button("D"))
        {
            size_t pos = data.editingPath.find_last_of('/');
            data.editingPath.erase(data.editingPath.begin() + pos, data.editingPath.end());
            data.currentPath = data.editingPath;
        }
        ImGui::SameLine();
        ImGui::PopFont();
        ImGui::SameLine();
        if(data.searchEnable == false)
        {
            ImGui::PushFont(iconFont);
            if(ImGui::Button("H"))
            {
                data.searchEnable = true;
            }
            ImGui::PopFont();
        }
        else
        {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText("##search", &data.searchItem);
        }

        if(data.currentPath.size() == 0)
        {
            data.currentPath = '/';
            data.editingPath = '/';
        }

        for (int i = 0; i < directories.size(); i++)
        {
            std::string viewString = directories[i].path();
            viewString.erase(viewString.begin(), viewString.begin() + viewString.find_last_of('/') + 1);

            ImGui::PushFont(iconFont);
            if(directories[i].is_directory())
            {
                ImGui::Text("B");
            }
            else
            {
                std::string extension = directories[i].path();
                extension.erase(extension.begin(), extension.begin() + extension.find_last_of('.') + 1);

                if(extension == "png" || extension == "jpg" || extension == "jpeg")
                    ImGui::Text("6");
                else if(extension == "cpp")
                    ImGui::Text("9");
                else if(extension == "hpp" || extension == "cpp")
                    ImGui::Text("9");
                else if(extension == "vert" || extension == "frag")
                    ImGui::Text("<");
                else if(extension == "bin" || extension == "spv")
                    ImGui::Text("=");
                else
                    ImGui::Text("I");
            }
            ImGui::PopFont();
            ImGui::SameLine();
            if(ImGui::Selectable(viewString.c_str(), i == data.selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                data.selected = i;
                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    data.selected = UINT32_MAX;
                    if(directories[i].is_directory())
                    {
                        data.editingPath = directories[i].path();
                        data.currentPath = data.editingPath;
                    }
                    else
                    {
                        filename = directories[i].path();
                        fileDialogDataMap.erase(label);
                        ImGui::End();
                        opened = false;
                        return true;
                    }
                }
            }
        }
        ImGui::End();

        if(ImGui::IsKeyPressed(ImGuiKey_DownArrow))
        {
            if(data.selected == UINT32_MAX)
            {
                data.selected = 0;
            }
            else
            {
                data.selected = (data.selected + 1) % directories.size();
            }
        }

        if(ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        {
            if(data.selected == UINT32_MAX)
            {
                data.selected = 0;
            }
            else if(data.selected == 0)
            {
                data.selected = directories.size() - 1;
            }
            else
            {
                data.selected = (data.selected - 1) % directories.size();
            }
        }

        return false;
    }

    void ClearFileDialogData(std::string_view label) 
    {
        fileDialogDataMap.erase(label.data());
    }

    void IconCharacter(char ch)
    {
        ImGui::PushFont(iconFont);
        
        ImGui::Text("%c", ch);
        ImGui::PopFont();
    }

    void IconCharacterSameLine(char ch)
    {
        ImGui::PushFont(iconFont);
        
        ImGui::Text("%c", ch);
        ImGui::PopFont();
        ImGui::SameLine();
    }
}