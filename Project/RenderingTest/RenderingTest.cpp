#include "Core/Macro.hpp"
#include "Renderer/Texture.hpp"
#include <Engine.hpp>
#include <Renderer/Renderer.hpp>

using namespace glm;

#include <glm/glm.hpp>

float perlin(vec3 p) 
{
    auto fade = [](float t) {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    };

    auto grad = [](int hash, glm::vec3 p) {
        switch (hash & 15) {
            case  0: return  p.x + p.y;
            case  1: return -p.x + p.y;
            case  2: return  p.x - p.y;
            case  3: return -p.x - p.y;
            case  4: return  p.x + p.z;
            case  5: return -p.x + p.z;
            case  6: return  p.x - p.z;
            case  7: return -p.x - p.z;
            case  8: return  p.y + p.z;
            case  9: return -p.y + p.z;
            case 10: return  p.y - p.z;
            case 11: return -p.y - p.z;
            case 12: return  p.y + p.x;
            case 13: return -p.y + p.z;
            case 14: return  p.y - p.x;
            case 15: return -p.y - p.z;
            default: return 0.0f;
        }
    };

    static const int perm[512] = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,
        69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,
        252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,
        171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,
        122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,
        161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
        159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,
        147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
        246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,
        121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
        128,195,78,66,215,61,156,180,
        // duplicated
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,
        69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,
        252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,
        171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,
        122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,
        161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
        159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,
        147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
        246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,
        121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
        128,195,78,66,215,61,156,180
    };

    glm::ivec3 i = glm::ivec3(glm::floor(p)) & 255;
    glm::vec3  f = glm::fract(p);
    glm::vec3  u = glm::vec3(fade(f.x), fade(f.y), fade(f.z));

    int aaa = perm[perm[perm[i.x    ] + i.y    ] + i.z    ];
    int aba = perm[perm[perm[i.x    ] + i.y + 1] + i.z    ];
    int aab = perm[perm[perm[i.x    ] + i.y    ] + i.z + 1];
    int abb = perm[perm[perm[i.x    ] + i.y + 1] + i.z + 1];
    int baa = perm[perm[perm[i.x + 1] + i.y    ] + i.z    ];
    int bba = perm[perm[perm[i.x + 1] + i.y + 1] + i.z    ];
    int bab = perm[perm[perm[i.x + 1] + i.y    ] + i.z + 1];
    int bbb = perm[perm[perm[i.x + 1] + i.y + 1] + i.z + 1];

    glm::vec3 f1 = f - glm::vec3(1.0f);

    float x1 = glm::mix(grad(aaa, f),                        grad(baa, {f1.x, f.y,  f.z }),  u.x);
    float x2 = glm::mix(grad(aba, {f.x, f1.y, f.z }),        grad(bba, {f1.x, f1.y, f.z }),  u.x);
    float x3 = glm::mix(grad(aab, {f.x, f.y,  f1.z}),        grad(bab, {f1.x, f.y,  f1.z}), u.x);
    float x4 = glm::mix(grad(abb, {f.x, f1.y, f1.z}),        grad(bbb, {f1.x, f1.y, f1.z}), u.x);

    float y1 = glm::mix(x1, x2, u.y);
    float y2 = glm::mix(x3, x4, u.y);

    return glm::mix(y1, y2, u.z);
}

float combinedPerlin(vec3 st)
{
	float l = 4;
	float p = 8;
	float result = 0;
	for (int i = 0; i < 10; i++)
	{
		result += perlin(st * vec3(pow(l,i))) / pow(p,i);
	}

	return result * 0.1;
}

class RenderingTest : public Application
{
	bool freeCamera = true;

	StaticMesh mMesh;
	Material mMaterial;
	Material mSkyMaterial;

	StaticMesh mGrassBlade;
	Material mGrassMaterial;

	StaticMesh mPlane;

	Camera mCamera;

	InstanceBuffer mInstanceBuffer;

	void Start() override
	{
		GetWindowRef().SetFullscreen(true);

		Vertex vertices[] =
		{
			// Front face (z = +0.5)
			{vec3( 0.5,  0.5,  0.5), vec2(1, 0), vec3( 0,  0,  1)},
			{vec3( 0.5, -0.5,  0.5), vec2(1, 1), vec3( 0,  0,  1)},
			{vec3(-0.5, -0.5,  0.5), vec2(0, 1), vec3( 0,  0,  1)},
			{vec3(-0.5,  0.5,  0.5), vec2(0, 0), vec3( 0,  0,  1)},

			// Back face (z = -0.5)
			{vec3(-0.5,  0.5, -0.5), vec2(1, 0), vec3( 0,  0, -1)},
			{vec3(-0.5, -0.5, -0.5), vec2(1, 1), vec3( 0,  0, -1)},
			{vec3( 0.5, -0.5, -0.5), vec2(0, 1), vec3( 0,  0, -1)},
			{vec3( 0.5,  0.5, -0.5), vec2(0, 0), vec3( 0,  0, -1)},

			// Left face (x = -0.5)
			{vec3(-0.5,  0.5,  0.5), vec2(1, 0), vec3(-1,  0,  0)},
			{vec3(-0.5, -0.5,  0.5), vec2(1, 1), vec3(-1,  0,  0)},
			{vec3(-0.5, -0.5, -0.5), vec2(0, 1), vec3(-1,  0,  0)},
			{vec3(-0.5,  0.5, -0.5), vec2(0, 0), vec3(-1,  0,  0)},

			// Right face (x = +0.5)
			{vec3( 0.5,  0.5, -0.5), vec2(1, 0), vec3( 1,  0,  0)},
			{vec3( 0.5, -0.5, -0.5), vec2(1, 1), vec3( 1,  0,  0)},
			{vec3( 0.5, -0.5,  0.5), vec2(0, 1), vec3( 1,  0,  0)},
			{vec3( 0.5,  0.5,  0.5), vec2(0, 0), vec3( 1,  0,  0)},

			// Top face (y = +0.5)
			{vec3( 0.5,  0.5, -0.5), vec2(1, 0), vec3( 0,  1,  0)},
			{vec3( 0.5,  0.5,  0.5), vec2(1, 1), vec3( 0,  1,  0)},
			{vec3(-0.5,  0.5,  0.5), vec2(0, 1), vec3( 0,  1,  0)},
			{vec3(-0.5,  0.5, -0.5), vec2(0, 0), vec3( 0,  1,  0)},

			// Bottom face (y = -0.5)
			{vec3( 0.5, -0.5,  0.5), vec2(1, 0), vec3( 0, -1,  0)},
			{vec3( 0.5, -0.5, -0.5), vec2(1, 1), vec3( 0, -1,  0)},
			{vec3(-0.5, -0.5, -0.5), vec2(0, 1), vec3( 0, -1,  0)},
			{vec3(-0.5, -0.5,  0.5), vec2(0, 0), vec3( 0, -1,  0)},
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


		GenerateGrassBlade(mGrassBlade);
		mGrassMaterial.LoadShaders("Shader/grass.vert.spv", "Shader/grass.frag.spv");
		mGrassMaterial.GetSettingsRef().cullMode = CullMode::None;	
		mGrassMaterial.GetSettingsRef().enableInstancing = true;

		mGrassMaterial.Create();

		GenerateCenteredPlane(mPlane, {1, 1}, {10.f, 10.f});

		mMesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));

		mMaterial.LoadShaders("Shader/shader.vert.spv", "Shader/shader.frag.spv");
		mMaterial.GetSettingsRef().cullMode = CullMode::None;
		mMaterial.Create();

		mSkyMaterial.LoadShaders("Shader/skybox.vert.spv", "Shader/skybox.frag.spv");
		mSkyMaterial.GetSettingsRef().cullMode = CullMode::None;
		mSkyMaterial.GetSettingsRef().depthTestEnable = false;
		mSkyMaterial.Create();


		mCamera.SetCameraType(CameraType::Orbital)
				.SetFront(vec3(0.0f))
				.SetPosition(vec3(0.f,0.f,2.f))
				.SetAspectRatio(float(GetWindowRef().GetSize().x) / float(GetWindowRef().GetSize().y))
				.SetNearPlane(0.01f)
				.SetFarPlane(1000.f);

		mTransform.position = vec3(0,0,0);
		mTransform.scale = vec3(1);

		GenerateGrassTransform({1000 ,1000 }, {0.2,0.2}, {-100, -100});

		
	}


	Transform mSkyTransform;

	uint32_t grassCount = 0;

	void GenerateGrassTransform(const glm::uvec2& size, const glm::vec2& spacing, const glm::vec2& offset)
	{
		std::vector<glm::mat4> instanceData; 
		
		grassCount = 0;

		for (int i = 0; i < size.x; i++)
		{
			for (int j = 0; j < size.y; j++)
			{
				Transform transform;
				float x = (drand48() - 0.5f) * 2.f;
				float z = (drand48() - 0.5f) * 2.f;
				float r = ((drand48() - 0.5f) * 2.f) * 360.f;

				transform.position = vec3(((spacing.x * i) + x) + offset.x, 0, ((spacing.y * j) + z) + offset.y);
				transform.rotation.y = r;

				instanceData.push_back(transform.GetMatrix());
				grassCount++;
			}
		}

		mInstanceBuffer.SetData(instanceData.data(), sizeof(glm::mat4) * instanceData.size());
	}

	void Update() override
	{
		Transform transform;
		transform.scale = vec3(100.f);
		ProcessCamera();
		mRenderer.SetCamera(mCamera);
		
		mRenderer.BeginFrame();
		mRenderer.DrawMeshWithMaterial(mMesh, mSkyMaterial, transform);
		mRenderer.DrawMeshWithMaterial(mPlane, mMaterial, transform);
		mRenderer.DrawMeshWithMaterialInstanced(mGrassBlade, mGrassMaterial, mInstanceBuffer, grassCount);
		mRenderer.EndFrame();
	}
	
	void End() override
	{
		vkDeviceWaitIdle(getDevice());
		mMesh.Destroy();
	}


	void OnWindowResize(const uvec2 &size) override
	{
		mRenderer.Resize(size);
		mCamera.SetAspectRatio(float(size.x) / float(size.y));
	}

	void ResizeView(const uvec2 &size)
	{
		mRenderer.Resize(size);
		mCamera.SetAspectRatio(float(size.x) / float(size.y));
	}
	
	float mYaw = 0, mPitch = 0;
	float mZoom = 2.f;
	void OnMouseMove(const vec2& position, const vec2& offset) override
	{
		if(!IsCursorHidden())
			return;
		mYaw -= offset.x;
		mPitch += offset.y;
	
		mPitch = clamp(mPitch, -89.f, 89.f);
	}
	
	void OnScroll(const vec2& scroll) override
	{
		if(!IsCursorHidden())
			return;
		mZoom += scroll.y;
	
		mZoom = clamp(mZoom, 0.01f, 100.f);
	}
	

	bool moveForward = false, moveBackward = false, moveLeft = false, moveRight = false, moveUp = false, moveDown = false;

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
		if(key == Key::F)
			freeCamera = !freeCamera;

		if(key == Key::W)
			moveForward = true;
		if(key == Key::S)
			moveBackward = true;
		if(key == Key::A)
			moveLeft = true;
		if(key == Key::D)
			moveRight = true;
		if(key == Key::Space)
			moveUp = true;
		if(key == Key::LeftShift)
			moveDown = true;
	}

	void OnKeyRelease(Key key) override
	{
		if(key == Key::W)
			moveForward = false;
		if(key == Key::S)
			moveBackward = false;
		if(key == Key::A)
			moveLeft = false;
		if(key == Key::D)
			moveRight = false;
		if(key == Key::Space)
			moveUp = false;
		if(key == Key::LeftShift)
			moveDown = false;
	}
	
	void ProcessCamera()
	{
		vec3 cameraPosition = mCamera.GetPosition();
		vec3 cameraFront = mCamera.GetFront();

		float speed = 0.05f;

		if(freeCamera)
		{
			mCamera.SetCameraType(CameraType::Perspective);

			cameraFront.x = sin(radians(mYaw)) * cos(radians(-mPitch));
			cameraFront.y = sin(radians(-mPitch));
			cameraFront.z = cos(radians(mYaw)) * cos(radians(-mPitch));


			vec3 forward = normalize(vec3(cameraFront.x, 0, cameraFront.z));
			vec3 side = normalize(cross(vec3(cameraFront.x, 0, cameraFront.z), mCamera.GetUp()));

			if(moveForward)
			{
				cameraPosition +=  forward * speed;
			}
			if(moveBackward)
			{
				cameraPosition -= forward * speed;
			}
			if(moveLeft)
			{
				cameraPosition -= side * speed;
			}
			if(moveRight)
			{
				cameraPosition += side * speed;
			}
			if(moveUp)
			{
				cameraPosition += mCamera.GetUp() * speed;
			}
			if(moveDown)
			{
				cameraPosition -= mCamera.GetUp() * speed;
			}

		}
		else
		{
			mCamera.SetCameraType(CameraType::Orbital);

			cameraPosition.x = sin(radians(mYaw)) * cos(radians(mPitch));
			cameraPosition.y = sin(radians(mPitch));
			cameraPosition.z = cos(radians(mYaw)) * cos(radians(mPitch));
			cameraPosition = cameraPosition * mZoom;
		}
	
	
		mCamera.SetFront(cameraFront);
		mCamera.SetPosition(cameraPosition);
	}

	void GenerateTerrain(StaticMesh& mesh, const ivec2& cellCount, const vec2& cellSize, const vec2& startPos)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		int planeIndex = 0;

		for (int i = 0; i < cellCount.x; i++)
		{
			for (int j = 0; j < cellCount.y; j++)
			{
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+1)), 0	, startPos.y + ( cellSize.y * (j+1))), vec2(1, 0), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+1)), 0	, startPos.y + ( cellSize.y * (j+0))), vec2(1, 1), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+0)), 0	, startPos.y + ( cellSize.y * (j+0))), vec2(0, 1), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+0)), 0	, startPos.y + ( cellSize.y * (j+0))), vec2(0, 1), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+0)), 0	, startPos.y + ( cellSize.y * (j+1))), vec2(0, 0), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+1)), 0	, startPos.y + ( cellSize.y * (j+1))), vec2(1, 0), vec3( 0,  0,  1)});

				(vertices.end() - 6)->position.y = combinedPerlin((vertices.end() - 6)->position);
				(vertices.end() - 5)->position.y = combinedPerlin((vertices.end() - 5)->position);
				(vertices.end() - 4)->position.y = combinedPerlin((vertices.end() - 4)->position);
				(vertices.end() - 3)->position.y = combinedPerlin((vertices.end() - 3)->position);
				(vertices.end() - 2)->position.y = combinedPerlin((vertices.end() - 2)->position);
				(vertices.end() - 1)->position.y = combinedPerlin((vertices.end() - 1)->position);

				vec3 vertex1 = (vertices.end() - 6)->position;			
				vec3 vertex2 = (vertices.end() - 5)->position;			
				vec3 vertex3 = (vertices.end() - 4)->position;

				vec3 vertex4 = (vertices.end() - 3)->position;			
				vec3 vertex5 = (vertices.end() - 2)->position;			
				vec3 vertex6 = (vertices.end() - 1)->position;

				vec3 normal1 = cross(normalize(vertex2 - vertex1), normalize(vertex3 - vertex1));
				vec3 normal2 = cross(normalize(vertex5 - vertex4), normalize(vertex6 - vertex4));


				(vertices.end() - 6)->normal = normal1;
				(vertices.end() - 5)->normal = normal1;
				(vertices.end() - 4)->normal = normal1;
				(vertices.end() - 3)->normal = normal2;
				(vertices.end() - 2)->normal = normal2;
				(vertices.end() - 1)->normal = normal2;

				
				indices.push_back(0 + (6 * planeIndex));
				indices.push_back(1 + (6 * planeIndex));
				indices.push_back(2 + (6 * planeIndex));
				indices.push_back(3 + (6 * planeIndex));
				indices.push_back(4 + (6 * planeIndex));
				indices.push_back(5 + (6 * planeIndex));
				
				planeIndex++;
			}
		}
		mesh.SetData(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), sizeof(uint32_t) * indices.size());
	}

	void GeneratePlane(StaticMesh& mesh, const ivec2& cellCount, const vec2& cellSize, const vec2& startPos)//int cellCount.x, int cellCount.y, float cellSize.x, float cellSize.y, float startPos.x, float startPos.y)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		int planeIndex = 0;

		for (int i = 0; i < cellCount.x; i++)
		{
			for (int j = 0; j < cellCount.y; j++)
			{
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+1)), 0	, startPos.y + ( cellSize.y * (j+1))), vec2(1, 0), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * (i+1)), 0	, startPos.y + ( cellSize.y * j)	), vec2(1, 1), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * i), 	  0	, startPos.y + ( cellSize.y * j)	), vec2(0, 1), vec3( 0,  0,  1)});
				vertices.push_back({vec3(startPos.x + ( cellSize.x * i), 	  0	, startPos.y + ( cellSize.y * (j+1))), vec2(0, 0), vec3( 0,  0,  1)});

				indices.push_back(0 + (4 * planeIndex));
				indices.push_back(1 + (4 * planeIndex));
				indices.push_back(2 + (4 * planeIndex));
				indices.push_back(2 + (4 * planeIndex));
				indices.push_back(3 + (4 * planeIndex));
				indices.push_back(0 + (4 * planeIndex));
				
				planeIndex++;
			}
		}
		mesh.SetData(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), sizeof(uint32_t) * indices.size());
	}

	void GenerateCenteredPlane(StaticMesh& mesh, const ivec2& cellCount, const vec2& cellSize)
	{
		GeneratePlane(mPlane, {10, 10}, {0.5, 0.5}, -GetPlaneCenter({10, 10}, {0.5, 0.5}));
	}

	vec2 GetPlaneCenter(const ivec2& cellCount, const vec2& cellSize)
	{
		return vec2((float(cellCount.x) * cellSize.x) * 0.5, (float(cellCount.y) * cellSize.y) * 0.5);
	}

	void GenerateGrassBlade(StaticMesh& mesh)
	{
		Vertex vertices[] = 
		{
			{vec3( -0.000000, 2.000000, 0.000000), vec2(0,1), vec3(1,0,0)},
			{vec3( 0.000000, 0.000000, 0.056885), vec2(0,0), vec3(1,0,0)},
			{vec3( 0.000000, 0.000000, -0.056885), vec2(0,0), vec3(1,0,0)},
			{vec3( -0.000000, 1.666667, 0.056885), vec2(0,0.8333), vec3(1,0,0)},
			{vec3( -0.000000, 1.333333, 0.056885), vec2(0,0.66665), vec3(1,0,0)},
			{vec3( 0.000000, 1.000000, 0.056885), vec2(0,0.5), vec3(1,0,0)},
			{vec3( 0.000000, 0.666667, 0.056885), vec2(0,0.3333), vec3(1,0,0)},
			{vec3( 0.000000, 0.333333, 0.056885), vec2(0,0.16665), vec3(1,0,0)},
			{vec3( 0.000000, 0.333333, -0.056885), vec2(0,0.16665), vec3(1,0,0)},
			{vec3( 0.000000, 0.666667, -0.056885), vec2(0,0.3333), vec3(1,0,0)},
			{vec3( -0.000000, 1.000000, -0.056885), vec2(0,0.5), vec3(1,0,0)},
			{vec3( -0.000000, 1.333333, -0.056885), vec2(0,0.66665), vec3(1,0,0)},
			{vec3( -0.000000, 1.666667, -0.056885), vec2(0,0.8333), vec3(1,0,0)},
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
	
	Transform mTransform;
	Transform mTransform1;
};

Application *Application::Create() { return new RenderingTest(); }