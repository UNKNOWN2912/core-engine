#include "MaterialManager.hpp"

MaterialID MaterialManager::LoadMaterial(std::string_view filename)
{
    MaterialID id = GenerateID();
    mMaterialMap[id] = std::make_shared<Material>();
    return id;
}

MaterialID MaterialManager::AddMaterial(std::shared_ptr<Material> material)
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
    mMaterialMap[materialId].reset();
}

std::shared_ptr<Material> MaterialManager::GetMaterial(MaterialID id)
{
    return mMaterialMap.contains(id) ? mMaterialMap.at(id) : nullptr;
}

bool MaterialManager::HasMaterial(MaterialID materialId)
{
    return GetMaterial(materialId) == nullptr;
}
void MaterialManager::Clear()
{
    mMaterialMap.clear();
}

const std::unordered_map<MaterialID, std::shared_ptr<Material>> &MaterialManager::GetMap()
{
    return MaterialManager::mMaterialMap;
}

uint64_t MaterialManager::mLastMaterialId = 0;
std::unordered_map<MaterialID, std::shared_ptr<Material>> MaterialManager::mMaterialMap;
