#pragma once
#include "Renderer/Mesh.hpp"
#include <memory>
#include <unordered_map>

class MeshManager
{
public:
    std::string CreateMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, std::string_view identifier);
    std::string AddMesh(const Mesh &mesh, std::string_view identifier);
    void DestroyMesh(std::string_view identifier);

    const Mesh &GetMesh(std::string_view identifier) const;
    Mesh &GetMesh(std::string_view identifier);
    bool HasMesh(std::string_view identifier) const;

    void Clear();

    const std::unordered_map<std::string, Mesh> &GetMap() const;

private:
    uint64_t mLastMeshId;
    std::unordered_map<std::string, Mesh> mMeshMap;
};
