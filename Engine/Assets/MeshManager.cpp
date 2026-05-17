#include "MeshManager.hpp"

std::shared_ptr<StaticMesh> MeshManager::AddMesh(std::string_view identifier, std::shared_ptr<StaticMesh> mesh)
{
    mMeshMap[identifier.data()] = mesh;
    return mesh;
}
void MeshManager::DestroyMesh(std::string_view identifier)
{
    mMeshMap[identifier.data()]->Destroy();
    mMeshMap[identifier.data()].reset();
}
std::shared_ptr<StaticMesh> MeshManager::GetMesh(std::string_view identifier)
{
    return mMeshMap[identifier.data()];
}
bool MeshManager::HasMesh(std::string_view identifier)
{
    return GetMesh(identifier) == nullptr;
}
