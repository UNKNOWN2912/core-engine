#pragma once
#include "Renderer/Mesh.hpp"
#include <memory>
#include <string_view>

class MeshManager
{
    public:
        std::shared_ptr<StaticMesh> AddMesh(std::string_view identifier, std::shared_ptr<StaticMesh> mesh);
        void DestroyMesh(std::string_view identifier);

        std::shared_ptr<StaticMesh> GetMesh(std::string_view identifier);
        bool HasMesh(std::string_view identifier);

    private:
        std::unordered_map<std::string, std::shared_ptr<StaticMesh>> mMeshMap; 
};