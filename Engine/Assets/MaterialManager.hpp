#pragma once
#include "Renderer/Material.hpp"

class MaterialManager
{
    public:
        std::shared_ptr<Material> LoadMaterial(std::string_view filename);
        std::shared_ptr<Material> AddMaterial(std::string_view identifier, std::shared_ptr<Material> material);

        void DestroyMaterial(std::string_view identifier);

        std::shared_ptr<Material> GetMaterial(std::string_view identifier);
        bool HasMaterial(std::string_view identifier);
    private:
        std::unordered_map<std::string, std::shared_ptr<Material>> mMaterialMap;
};