#pragma once
#include "Renderer/Material.hpp"

class MaterialManager
{
public:
    static std::string LoadMaterial(std::string_view filename, std::string_view identifier);
    static std::string AddMaterial(const Material &material, std::string_view identifier);

    static void DestroyMaterial(std::string_view identifier);

    static Material &GetMaterial(std::string_view identifier);
    static bool HasMaterial(std::string_view identifier);

    static void Clear();

    static const std::unordered_map<std::string, Material> &GetMap();

private:
    static uint64_t mLastMaterialId;
    static std::unordered_map<std::string, Material> mMaterialMap;
};