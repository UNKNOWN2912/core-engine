#include "Core/Macro.hpp"
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
	Camera mCamera;

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

		mCamera.SetCameraType(CameraType::Orbital);
		mCamera.SetFront(glm::vec3(0.0f));
		mCamera.SetPosition(glm::vec3(0.f,0.f,2.f));
	}

	void OnWindowResize(const glm::uvec2 &size) override
	{
		mCamera.SetAspectRatio(float(size.x) / float(size.y));
	}

	float yaw = 0, pitch = 0;
	float zoom = 1.f;
	void OnMouseMove(const glm::vec2& position, const glm::vec2& offset) override
	{
		yaw += offset.x;
		pitch -= offset.y;

		pitch = glm::clamp(pitch, -89.f, 89.f);
	}

	void OnScroll(const glm::vec2& scroll) override
	{
		zoom += scroll.y;

		zoom = glm::clamp(zoom, 0.01f, 100.f);
	}

	void OnKeyPress(Key key) override
	{
	}

	void OnCharacterType(char ch) override
	{
		std::println("{}", ch);
	}

	void ProcessCamera()
	{
		glm::vec3 cameraPosition = mCamera.GetPosition();


		cameraPosition.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraPosition.y = sin(glm::radians(pitch));
		cameraPosition.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

		cameraPosition = cameraPosition * zoom;


		mCamera.SetPosition(cameraPosition);

	}

	void Update() override
	{
		ProcessCamera();
		mRenderer.SetCamera(mCamera);

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