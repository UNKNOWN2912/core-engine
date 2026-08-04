#include "MeshManager.hpp"

std::string MeshManager::CreateMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, std::string_view identifier)
{
    Mesh mesh(vertices, indices);
    return AddMesh(mesh, identifier);
}

std::string MeshManager::AddMesh(const Mesh &mesh, std::string_view identifier)
{
    mMeshMap[identifier.data()] = mesh;
    mMeshMap[identifier.data()].SetName(identifier.data());
    return identifier.data();
}
void MeshManager::Clear()
{
    mMeshMap.clear();
}
std::unordered_map<std::string, Mesh> &MeshManager::GetMap()
{
    return mMeshMap;
}

void MeshManager::DestroyMesh(std::string_view identifier)
{
    mMeshMap[identifier.data()].Destroy();
}

const Mesh &MeshManager::GetMesh(std::string_view identifier)
{
    assert(HasMesh(identifier));
    return mMeshMap.at(identifier.data());
}

Mesh &MeshManager::GetMeshRef(std::string_view identifier)
{
    assert(HasMesh(identifier));
    return mMeshMap.at(identifier.data());
}

bool MeshManager::HasMesh(std::string_view identifier)
{
    return mMeshMap.contains(identifier.data());
}

uint64_t MeshManager::mLastMeshId = 0;
std::unordered_map<std::string, Mesh> MeshManager::mMeshMap;
