#pragma once
#include <memory>
#include <Renderer/Mesh.hpp>
#include <Renderer/Material.hpp>

class MeshRenderer
{
public:
	void SetMesh(std::shared_ptr<Mesh> mesh);
	void SetMaterial(std::shared_ptr<Material> material);

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();

	MeshRenderer(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
private:
	std::shared_ptr<Mesh> mMesh;
	std::shared_ptr<Material> mMaterial;
};


