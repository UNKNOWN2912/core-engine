#pragma once
#include "Renderer/Mesh.hpp"
#include <memory>
#include <unordered_map>

class MeshManager
{
public:
    static std::string CreateMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, std::string_view identifier);
    static std::string AddMesh(const Mesh &mesh, std::string_view identifier);
    static void DestroyMesh(std::string_view identifier);

    static const Mesh &GetMesh(std::string_view identifier);
    static Mesh &GetMeshRef(std::string_view identifier);
    static bool HasMesh(std::string_view identifier);

    static void Clear();

    static std::unordered_map<std::string, Mesh> &GetMap();

private:
    static uint64_t mLastMeshId;
    static std::unordered_map<std::string, Mesh> mMeshMap;
};
