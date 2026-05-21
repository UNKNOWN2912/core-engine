#include "MeshManager.hpp"

MeshID MeshManager::AddMesh(std::shared_ptr<StaticMesh> mesh)
{
    MeshID id = GenerateID();
    mMeshMap[id] = mesh;
    return id;
}
MeshID MeshManager::GenerateID()
{
    return (MeshID)mLastMeshId++;
}
void MeshManager::DestroyMesh(MeshID id)
{
    mMeshMap[id]->Destroy();
    mMeshMap[id].reset();
}
std::shared_ptr<StaticMesh> MeshManager::GetMesh(MeshID id)
{
    return mMeshMap[id];
}
bool MeshManager::HasMesh(MeshID id)
{
    return mMeshMap.contains(id);
}

uint64_t MeshManager::mLastMeshId = 0;
std::unordered_map<MeshID, std::shared_ptr<StaticMesh>> MeshManager::mMeshMap;
