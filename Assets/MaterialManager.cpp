#include "MaterialManager.hpp"

MaterialID MaterialManager::LoadMaterial(std::string_view filename)
{
    MaterialID id = GenerateID();
    mMaterialMap[id] = Material();
    return id;
}

MaterialID MaterialManager::AddMaterial(const Material &material)
{
    MaterialID id = GenerateID();
    mMaterialMap[id] = material;
    return id;
}

MaterialID MaterialManager::GenerateID()
{
    return (MaterialID)mLastMaterialId++;
}

void MaterialManager::DestroyMaterial(MaterialID materialId)
{
    mMaterialMap[materialId] = {};
}

Material &MaterialManager::GetMaterial(MaterialID id)
{
    return mMaterialMap.at(id);
}

bool MaterialManager::HasMaterial(MaterialID materialId)
{
    return mMaterialMap.contains(materialId);
}
void MaterialManager::Clear()
{
    mMaterialMap.clear();
}

const std::unordered_map<MaterialID, Material> &MaterialManager::GetMap()
{
    return MaterialManager::mMaterialMap;
}

uint64_t MaterialManager::mLastMaterialId = 0;
std::unordered_map<MaterialID, Material> MaterialManager::mMaterialMap;
