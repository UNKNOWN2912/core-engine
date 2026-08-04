#include "MaterialManager.hpp"

std::string MaterialManager::LoadMaterial(std::string_view filename, std::string_view identifier)
{
    mMaterialMap[identifier.data()] = Material();
    return identifier.data();
}

std::string MaterialManager::AddMaterial(const Material &material, std::string_view identifier)
{
    mMaterialMap[identifier.data()] = material;
    return identifier.data();
}

void MaterialManager::DestroyMaterial(std::string_view identifier)
{
    mMaterialMap[identifier.data()] = {};
}

Material &MaterialManager::GetMaterial(std::string_view identifier)
{
    return mMaterialMap.at(identifier.data());
}

bool MaterialManager::HasMaterial(std::string_view identifier)
{
    return mMaterialMap.contains(identifier.data());
}
void MaterialManager::Clear()
{
    mMaterialMap.clear();
}

const std::unordered_map<std::string, Material> &MaterialManager::GetMap()
{
    return MaterialManager::mMaterialMap;
}

std::unordered_map<std::string, Material> MaterialManager::mMaterialMap;
