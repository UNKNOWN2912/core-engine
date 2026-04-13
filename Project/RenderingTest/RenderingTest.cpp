#include <Engine.hpp>
#include <Renderer/Renderer.hpp>

struct UniformBufferData
{
	glm::mat4 model = glm::mat4(1.f), view = glm::mat4(1.f), projection = glm::mat4(1.f);
};

struct Vertex
{
	glm::vec3 position;
	glm::vec2 uv;
};

class RenderingTest : public Application
{

	StaticMesh mMesh;
	
	void Start() override
	{
		glm::vec3 vertices[] = 
		{
			glm::vec3( 0.5, 0.5, 0.0),
			glm::vec3( 0.5,-0.5, 0.0),
			glm::vec3(-0.5,-0.5, 0.0),
		};

		uint32_t indices[] = 
		{
			0,1,2
		};

		mMesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));
		mMesh.SetLayout({LayoutType::Vec3});
	}

	void Update() override
	{
		mRenderer.BeginFrame();
		mRenderer.DrawMesh(mMesh);
		mRenderer.EndFrame();
	}

	void End() override
	{
    	vkDeviceWaitIdle(getDevice());
		mMesh.Destroy();
	}
};

Application *Application::Create() { return new RenderingTest(); }