#pragma once
#include "Renderer/Mesh.hpp"
#include <memory>
#include <unordered_map>

enum class MeshID : uint64_t;
#define INVALID_MESH_ID MeshID(UINT64_MAX);

class MeshManager
{
public:
    static MeshID CreateMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, std::string_view name = "");
    static MeshID AddMesh(std::shared_ptr<Mesh> mesh);
    static void DestroyMesh(MeshID id);

    static std::shared_ptr<Mesh> GetMesh(MeshID id);
    static bool HasMesh(MeshID id);

    static MeshID GenerateID();

    static void Clear();

    static std::unordered_map<MeshID, std::shared_ptr<Mesh>> &GetMap();

private:
    static uint64_t mLastMeshId;
    static std::unordered_map<MeshID, std::shared_ptr<Mesh>> mMeshMap;
};
