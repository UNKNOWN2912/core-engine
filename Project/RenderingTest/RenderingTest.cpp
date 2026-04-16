#include "CameraController.hpp"
#include "Core/Macro.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/Texture.hpp"
#include <Engine.hpp>
#include <Renderer/Renderer.hpp>
#include "ChunkManager.hpp"

class RenderingTest : public Application
{
	StaticMesh mMesh;
	StaticMesh mGrassBlade;
	StaticMesh mPlane;
	
	Material mMaterial;
	Material mSkyMaterial;
	Material mGrassMaterial;

	Camera mCamera;
	CameraController mCameraController;

	GrassChunkManager mGrassChunkManager;
	TerrainChunkManager mTerrainChunkManager;

	uint32_t mRenderDistance = 3;


	void Start() override
	{
		GetWindowRef().SetFullscreen(true);

		mCameraController.SetCamera(mCamera, GetWindowRef());

		CreateMaterials();
		CreateMeshes();

		for(int i = 0; i <= mRenderDistance; i++)
		{
			GenerateWorldAtRenderDistance(i);
		}

		mTransform.position = glm::vec3(0,0,0);
		mTransform.scale = glm::vec3(1);
	}

	void Update() override
	{
		mCameraController.Update();

		mRenderer.SetCamera(mCamera);
		mRenderer.BeginFrame();
		OnRender();
		mRenderer.EndFrame();
	}

	void OnRender()
	{
		mRenderer.DrawMeshWithMaterial(mMesh, mSkyMaterial, Transform());
		mGrassChunkManager.Draw(mGrassBlade, mGrassMaterial);
		mTerrainChunkManager.Draw(mMaterial);
	}
	
	void End() override
	{
		vkDeviceWaitIdle(getDevice());
		mMesh.Destroy();
	}


	void GenerateWorldAtRenderDistance(uint32_t renderDistance)
	{
		float t = glfwGetTime();
		glm::ivec2 topLeft 		= 	glm::ivec2(-renderDistance,-renderDistance);
		glm::ivec2 topRight 	= 	glm::ivec2(-renderDistance, renderDistance);
		glm::ivec2 bottomLeft 	= 	glm::ivec2( renderDistance,-renderDistance);
		glm::ivec2 bottomRight 	= 	glm::ivec2( renderDistance, renderDistance);

		for (int i = topLeft.y; i <= topRight.y; i++)
		{
			mGrassChunkManager.GenerateChunk({topLeft.x, i});
			mTerrainChunkManager.GenerateChunk({topLeft.x, i});
		}

		for (int i = topLeft.x + 1; i <= bottomLeft.x; i++)
		{
			mGrassChunkManager.GenerateChunk({i, topLeft.y});
			mTerrainChunkManager.GenerateChunk({i, topLeft.y});
		}

		for (int i = topRight.x + 1; i <= bottomRight.x; i++)
		{
			mGrassChunkManager.GenerateChunk({i, topRight.y});
			mTerrainChunkManager.GenerateChunk({i, topRight.y});
		}

		for (int i = bottomLeft.y + 1; i < bottomRight.y; i++)
		{
			mGrassChunkManager.GenerateChunk({bottomRight.x, i});
			mTerrainChunkManager.GenerateChunk({bottomRight.x, i});
		}
		LOG("Generation Time[{}]: {}", renderDistance, glfwGetTime() - t);
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

		if(key == Key::Equal)
		{
			mRenderDistance++;
			GenerateWorldAtRenderDistance(mRenderDistance);
		}
		if(key == Key::Minus)
		{
			mRenderDistance--;
			GenerateWorldAtRenderDistance(mRenderDistance);
		}

	}

	void CreateGrassBladeMesh(StaticMesh& mesh)
	{
		Vertex vertices[] = 
		{
			{glm::vec3( -0.000000, 2.000000, 0.000000), glm::vec2(0,1), glm::vec3(1,0,0)},
			{glm::vec3( 0.000000, 0.000000, 0.056885), glm::vec2(0,0), glm::vec3(1,0,0)},
			{glm::vec3( 0.000000, 0.000000, -0.056885), glm::vec2(0,0), glm::vec3(1,0,0)},
			{glm::vec3( -0.000000, 1.666667, 0.056885), glm::vec2(0,0.8333), glm::vec3(1,0,0)},
			{glm::vec3( -0.000000, 1.333333, 0.056885), glm::vec2(0,0.66665), glm::vec3(1,0,0)},
			{glm::vec3( 0.000000, 1.000000, 0.056885), glm::vec2(0,0.5), glm::vec3(1,0,0)},
			{glm::vec3( 0.000000, 0.666667, 0.056885), glm::vec2(0,0.3333), glm::vec3(1,0,0)},
			{glm::vec3( 0.000000, 0.333333, 0.056885), glm::vec2(0,0.16665), glm::vec3(1,0,0)},
			{glm::vec3( 0.000000, 0.333333, -0.056885), glm::vec2(0,0.16665), glm::vec3(1,0,0)},
			{glm::vec3( 0.000000, 0.666667, -0.056885), glm::vec2(0,0.3333), glm::vec3(1,0,0)},
			{glm::vec3( -0.000000, 1.000000, -0.056885), glm::vec2(0,0.5), glm::vec3(1,0,0)},
			{glm::vec3( -0.000000, 1.333333, -0.056885), glm::vec2(0,0.66665), glm::vec3(1,0,0)},
			{glm::vec3( -0.000000, 1.666667, -0.056885), glm::vec2(0,0.8333), glm::vec3(1,0,0)},
		};

		uint32_t indices[] = 
		{
			8-1, 3 - 1, 9 - 1,
			1-1, 4 - 1, 13 - 1,
			4-1, 12 - 1, 13 - 1,
			5-1, 11 - 1, 12 - 1,
			6-1, 10 - 1, 11 - 1,
			7-1, 9 - 1, 10 - 1,
			8-1, 2 - 1, 3 - 1,
			4-1, 5 - 1, 12 - 1,
			5-1, 6 - 1, 11 - 1,
			6-1, 7 - 1, 10 - 1,
			7-1, 8 - 1, 9 - 1,
		};
		
		mGrassBlade.SetData(vertices, sizeof(vertices), indices, sizeof(indices));
	}
	void CreateMeshes()
	{
		CreateGrassBladeMesh(mGrassBlade);
		CreateCubeMesh(mMesh);
	}

	void CreateCubeMesh(StaticMesh& mesh)
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

		mesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));
	}

	void CreateMaterials()
	{
		mGrassMaterial.LoadShaders("Shader/grass.vert.spv", "Shader/grass.frag.spv");
		mGrassMaterial.GetSettingsRef().cullMode = CullMode::None;
		mGrassMaterial.GetSettingsRef().enableInstancing = true;
		mGrassMaterial.Create();

		mMaterial.LoadShaders("Shader/shader.vert.spv", "Shader/shader.frag.spv");
		mMaterial.GetSettingsRef().cullMode = CullMode::None;
		mMaterial.Create();

		mSkyMaterial.LoadShaders("Shader/skybox.vert.spv", "Shader/skybox.frag.spv");
		mSkyMaterial.GetSettingsRef().cullMode = CullMode::None;
		mSkyMaterial.GetSettingsRef().depthTestEnable = false;
		mSkyMaterial.Create();
	}

	
	Transform mTransform;
	Transform mTransform1;
};

Application *Application::Create() { return new RenderingTest(); }