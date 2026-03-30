#include <Engine.hpp>
#include <Maths/Vector.hpp>
#include <filesystem>
#include <print>

class Sandbox : public Application
{
	void Initialize() override
	{
	}
	void Start() override 
	{

		GetAssetManager()->LoadShader("skybox", "Shader/shader.vert.spv", "Shader/shader.frag.spv");

		mesh = std::make_shared<Mesh>();
		material = std::make_shared<Material>();
		meshRenderer = std::make_shared<MeshRenderer>(mesh, material);

		mesh2 = std::make_shared<Mesh>();
		material2 = std::make_shared<Material>();
		meshRenderer2 = std::make_shared<MeshRenderer>(mesh2, material2);

		mesh->positions.push_back(Vector3f( 0.5, 0.5, 0.0));
		mesh->positions.push_back(Vector3f( 0.5,-0.5, 0.0));
		mesh->positions.push_back(Vector3f(-0.5,-0.5, 0.0));
		
		mesh->AddFace(0, 1, 2);
		
		material->shader = "skybox";

		mesh2->positions.push_back(Vector3f( 0.1, 0.1, 0.0));
		mesh2->positions.push_back(Vector3f( 0.1,-0.1, 0.0));
		mesh2->positions.push_back(Vector3f(-0.1,-0.1, 0.0));
		
		mesh2->AddFace(0, 1, 2);
		
		material2->shader = "skybox";

		
		GetRenderer()->CreateAssets(GetAssetManager());
		GetRenderer()->CreateMeshRendererObjects(meshRenderer);
		GetRenderer()->CreateMeshRendererObjects(meshRenderer2);
	}
	void Update() override 
	{
		GetRenderer()->BeginFrame(Vector4f(1,0,1,1));
		GetRenderer()->Submit(meshRenderer2);
		GetRenderer()->EndFrame();
	}
	void End() override 
	{
	}
};

Application *Application::Create() { return new Sandbox; }