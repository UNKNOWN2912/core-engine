#include "MaterialManager.hpp"

std::shared_ptr<Material> MaterialManager::LoadMaterial(std::string_view filename) 
{
    
}

std::shared_ptr<Material> MaterialManager::AddMaterial(std::string_view identifier, std::shared_ptr<Material> material) 
{
    mMaterialMap[identifier.data()] = material;    
    return material;
}

void MaterialManager::DestroyMaterial(std::string_view identifier) 
{
    mMaterialMap[identifier.data()]->Destroy();
    mMaterialMap[identifier.data()].reset();
}

std::shared_ptr<Material> MaterialManager::GetMaterial(std::string_view identifier) 
{
    return mMaterialMap[identifier.data()];    
}

bool MaterialManager::HasMaterial(std::string_view identifier) 
{
    return GetMaterial(identifier) == nullptr;    
}

