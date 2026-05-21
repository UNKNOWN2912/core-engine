#pragma once
#include "Renderer/Mesh.hpp"
#include <memory>
#include <unordered_map>

enum class MeshID : uint64_t;

class MeshManager
{
  public:
    static MeshID AddMesh(std::shared_ptr<StaticMesh> mesh);
    static void DestroyMesh(MeshID id);

    static std::shared_ptr<StaticMesh> GetMesh(MeshID id);
    static bool HasMesh(MeshID id);

    static MeshID GenerateID();

  private:
    static uint64_t mLastMeshId;
    static std::unordered_map<MeshID, std::shared_ptr<StaticMesh>> mMeshMap;
};