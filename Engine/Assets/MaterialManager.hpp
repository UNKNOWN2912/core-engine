#pragma once
#include "Renderer/Material.hpp"

enum class MaterialID : uint64_t;
#define INVALID_MATERIAL_ID MaterialID(UINT64_MAX);

class MaterialManager
{
public:
    static MaterialID LoadMaterial(std::string_view filename);
    static MaterialID AddMaterial(std::shared_ptr<Material> material);

    static MaterialID GenerateID();

    static void DestroyMaterial(MaterialID materialId);

    static std::shared_ptr<Material> GetMaterial(MaterialID materialId);
    static bool HasMaterial(MaterialID materialId);

    static void Clear();

    static const std::unordered_map<MaterialID, std::shared_ptr<Material>> &GetMap();

private:
    static uint64_t mLastMaterialId;
    static std::unordered_map<MaterialID, std::shared_ptr<Material>> mMaterialMap;
};