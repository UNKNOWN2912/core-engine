#pragma once
#include "Renderer/Material.hpp"

class MaterialManager
{
public:
    std::string LoadMaterial(std::string_view filename, std::string_view identifier);
    std::string AddMaterial(const Material &material, std::string_view identifier);

    void DestroyMaterial(std::string_view identifier);

    Material &GetMaterial(std::string_view identifier);
    bool HasMaterial(std::string_view identifier);

    void Clear();

    const std::unordered_map<std::string, Material> &GetMap() const;

private:
    uint64_t mLastMaterialId;
    std::unordered_map<std::string, Material> mMaterialMap;
};