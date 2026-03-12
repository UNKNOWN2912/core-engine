#include <Renderer/MeshRenderer.hpp>

void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh)
{
	mMesh = mesh;
}

void MeshRenderer::SetMaterial(std::shared_ptr<Material> material)
{
	mMaterial = material;
}
MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
	: mMesh(mesh),
	  mMaterial(material)
{
	
}
std::shared_ptr<Mesh> MeshRenderer::GetMesh() { return mMesh; }
std::shared_ptr<Material> MeshRenderer::GetMaterial() { return mMaterial; }
