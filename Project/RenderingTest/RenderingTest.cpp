#include "Core/Macro.hpp"
#include "Renderer/Texture.hpp"
#include <Engine.hpp>
#include <Renderer/Renderer.hpp>


class RenderingTest : public Application
{

	StaticMesh mMesh;
	Material mMaterial;
	Material mSkyMaterial;

	Camera mCamera;
	void Start() override
	{

		Vertex vertices[] =
		{
			// Front face (z = +0.5)
			{glm::vec3( 0.5,  0.5,  0.5), glm::vec2(1, 0), glm::vec3( 0,  0,  1)},
			{glm::vec3( 0.5, -0.5,  0.5), glm::vec2(1, 1), glm::vec3( 0,  0,  1)},
			{glm::vec3(-0.5, -0.5,  0.5), glm::vec2(0, 1), glm::vec3( 0,  0,  1)},
			{glm::vec3(-0.5,  0.5,  0.5), glm::vec2(0, 0), glm::vec3( 0,  0,  1)},

			// Back face (z = -0.5)
			{glm::vec3(-0.5,  0.5, -0.5), glm::vec2(1, 0), glm::vec3( 0,  0, -1)},
			{glm::vec3(-0.5, -0.5, -0.5), glm::vec2(1, 1), glm::vec3( 0,  0, -1)},
			{glm::vec3( 0.5, -0.5, -0.5), glm::vec2(0, 1), glm::vec3( 0,  0, -1)},
			{glm::vec3( 0.5,  0.5, -0.5), glm::vec2(0, 0), glm::vec3( 0,  0, -1)},

			// Left face (x = -0.5)
			{glm::vec3(-0.5,  0.5,  0.5), glm::vec2(1, 0), glm::vec3(-1,  0,  0)},
			{glm::vec3(-0.5, -0.5,  0.5), glm::vec2(1, 1), glm::vec3(-1,  0,  0)},
			{glm::vec3(-0.5, -0.5, -0.5), glm::vec2(0, 1), glm::vec3(-1,  0,  0)},
			{glm::vec3(-0.5,  0.5, -0.5), glm::vec2(0, 0), glm::vec3(-1,  0,  0)},

			// Right face (x = +0.5)
			{glm::vec3( 0.5,  0.5, -0.5), glm::vec2(1, 0), glm::vec3( 1,  0,  0)},
			{glm::vec3( 0.5, -0.5, -0.5), glm::vec2(1, 1), glm::vec3( 1,  0,  0)},
			{glm::vec3( 0.5, -0.5,  0.5), glm::vec2(0, 1), glm::vec3( 1,  0,  0)},
			{glm::vec3( 0.5,  0.5,  0.5), glm::vec2(0, 0), glm::vec3( 1,  0,  0)},

			// Top face (y = +0.5)
			{glm::vec3( 0.5,  0.5, -0.5), glm::vec2(1, 0), glm::vec3( 0,  1,  0)},
			{glm::vec3( 0.5,  0.5,  0.5), glm::vec2(1, 1), glm::vec3( 0,  1,  0)},
			{glm::vec3(-0.5,  0.5,  0.5), glm::vec2(0, 1), glm::vec3( 0,  1,  0)},
			{glm::vec3(-0.5,  0.5, -0.5), glm::vec2(0, 0), glm::vec3( 0,  1,  0)},

			// Bottom face (y = -0.5)
			{glm::vec3( 0.5, -0.5,  0.5), glm::vec2(1, 0), glm::vec3( 0, -1,  0)},
			{glm::vec3( 0.5, -0.5, -0.5), glm::vec2(1, 1), glm::vec3( 0, -1,  0)},
			{glm::vec3(-0.5, -0.5, -0.5), glm::vec2(0, 1), glm::vec3( 0, -1,  0)},
			{glm::vec3(-0.5, -0.5,  0.5), glm::vec2(0, 0), glm::vec3( 0, -1,  0)},
		};

		uint32_t indices[] =
		{
			0,  1,  2,  2,  3,  0,  
			4,  5,  6,  6,  7,  4,  
			8,  9,  10, 10, 11, 8,  
			12, 13, 14, 14, 15, 12, 
			16, 17, 18, 18, 19, 16, 
			20, 21, 22, 22, 23, 20, 
		};

		mMesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));

		mMaterial.LoadShaders("Shader/shader.vert.spv", "Shader/shader.frag.spv");
		mMaterial.LoadAlbedo("Texture/texture_01.png");
		mMaterial.GetSettingsRef().cullMode = CullMode::Back;
		mMaterial.Create();

		mSkyMaterial.LoadShaders("Shader/skybox.vert.spv", "Shader/skybox.frag.spv");
		mSkyMaterial.GetSettingsRef().cullMode = CullMode::None;
		mSkyMaterial.GetSettingsRef().depthTestEnable = false;
		mSkyMaterial.Create();


		mCamera.SetCameraType(CameraType::Orbital)
				.SetFront(glm::vec3(0.0f))
				.SetPosition(glm::vec3(0.f,0.f,2.f))
				.SetAspectRatio(float(GetWindowRef().GetSize().x) / float(GetWindowRef().GetSize().y))
				.SetNearPlane(0.01f)
				.SetFarPlane(100.f);

		mTransform.position = glm::vec3(0,-10,0);
		mTransform.scale = glm::vec3(100,1,100);

		
	}

	Transform mSkyTransform;

	void Update() override
	{
		ProcessCamera();
		mRenderer.SetCamera(mCamera);
		
		mRenderer.BeginFrame();
		mRenderer.DrawMeshWithMaterial(mMesh, mSkyMaterial, mSkyTransform);
		mRenderer.DrawMeshWithMaterial(mMesh, mMaterial, mTransform);
		mRenderer.DrawMeshWithMaterial(mMesh, mMaterial, mTransform1);
		mRenderer.EndFrame();
		
	}
	
	void End() override
	{
		vkDeviceWaitIdle(getDevice());
		mMesh.Destroy();
	}


	void OnWindowResize(const glm::uvec2 &size) override
	{
		mRenderer.Resize(size);
		mCamera.SetAspectRatio(float(size.x) / float(size.y));
	}

	void ResizeView(const glm::uvec2 &size)
	{
		mRenderer.Resize(size);
		mCamera.SetAspectRatio(float(size.x) / float(size.y));
	}
	
	float mYaw = 0, mPitch = 0;
	float mZoom = 2.f;
	void OnMouseMove(const glm::vec2& position, const glm::vec2& offset) override
	{
		if(!IsCursorHidden())
			return;
		mYaw -= offset.x;
		mPitch += offset.y;
	
		mPitch = glm::clamp(mPitch, -89.f, 89.f);
	}
	
	void OnScroll(const glm::vec2& scroll) override
	{
		if(!IsCursorHidden())
			return;
		mZoom += scroll.y;
	
		mZoom = glm::clamp(mZoom, 0.01f, 100.f);
	}
	
	void OnKeyPress(Key key) override
	{
		if(key == Key::E)
			ToggleCursor();
		if(key == Key::Escape)
			Close();
		if(key == Key::R)
			ResizeView({1920, 1080});
		if(key == Key::F11)
			GetWindowRef().SetFullscreen(!GetWindowRef().isFullscreen());
	}
	
	void ProcessCamera()
	{
		glm::vec3 cameraPosition = mCamera.GetPosition();
	
		cameraPosition.x = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		cameraPosition.y = sin(glm::radians(mPitch));
		cameraPosition.z = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	
		cameraPosition = cameraPosition * mZoom;
	
	
		mCamera.SetPosition(cameraPosition);
	}
	
	Transform mTransform;
	Transform mTransform1;
};

Application *Application::Create() { return new RenderingTest(); }