#include "MeshManager.hpp"

MeshID MeshManager::CreateMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, std::string_view name)
{
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    mesh->SetData(vertices, indices);
    mesh->SetName(name.data());
    return AddMesh(mesh);
}

MeshID MeshManager::AddMesh(std::shared_ptr<Mesh> mesh)
{
    MeshID id = GenerateID();
    mMeshMap[id] = mesh;
    return id;
}
MeshID MeshManager::GenerateID()
{
    return (MeshID)mLastMeshId++;
}
void MeshManager::Clear()
{
    mMeshMap.clear();
}
std::unordered_map<MeshID, std::shared_ptr<Mesh>> &MeshManager::GetMap()
{
    return mMeshMap;
}

void MeshManager::DestroyMesh(MeshID id)
{
    mMeshMap[id]->Destroy();
    mMeshMap[id].reset();
}
std::shared_ptr<Mesh> MeshManager::GetMesh(MeshID id)
{
    return (mMeshMap.contains(id)) ? mMeshMap.at(id) : nullptr;
}
bool MeshManager::HasMesh(MeshID id)
{
    return mMeshMap.contains(id);
}

uint64_t MeshManager::mLastMeshId = 0;
std::unordered_map<MeshID, std::shared_ptr<Mesh>> MeshManager::mMeshMap;
