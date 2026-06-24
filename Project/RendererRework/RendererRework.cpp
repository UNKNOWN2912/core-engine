#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Assets/ModelImporter.hpp"
#include "CameraController.hpp"
#include "EntityComponentSystem/Component.hpp"
#include <Engine.hpp>
#include <thread>

#define BindCommandCallback(callback) std::bind(&callback, this, std::placeholders::_1)

class RendererRework : public Application
{
    Surface mSurface;

    Camera mCamera;
    CameraController mController;

    Scene mScene;

    std::thread mCommandLineThread;

    std::vector<Light *> mLights;

    void OnInitialize() override
    {
        int scale = 120;
        Renderer::SetResolution({16 * scale, 9 * scale});
    }

    void OnStart() override
    {
        mCamera.SetFov(90.f);

        HideCursor();

        mController.SetSensitivity(0.1f);

        GetWindow().SetTitle("Renderer Rework");
        GetWindow().SetFullscreen(true);

        Renderer::SetBasicShader("Shaders/test.vert.spv", "Shaders/test.frag.spv");

        mController.SetCamera(mCamera, GetWindow());

        mSurface = Renderer::CreateSurface(GetWindow());

        ModelImporter importer;
        importer.Import("Models/cube/cube.gltf", mScene);
        importer.Import("Models/cube/environment.gltf", mScene);

        std::shared_ptr<Material> skyboxMaterial = std::make_shared<Material>();
        skyboxMaterial->vertexShader = ShaderManager::LoadVertexShader("Shaders/skybox.vert.spv");
        skyboxMaterial->fragmentShader = ShaderManager::LoadFragmentShader("Shaders/skybox.frag.spv");
        skyboxMaterial->cullMode = CullMode::None;
        skyboxMaterial->enableDepthTest = false;
        skyboxMaterial->enableDepthWrite = false;
        skyboxMaterial->name = "skybox";

        Renderer::RegisterMaterial(*skyboxMaterial);
        Entity entity = mScene.GetEntityByName("CubeCube");
        entity.GetComponent<MeshRendererComponent>().material = MaterialManager::AddMaterial(skyboxMaterial);

        for (auto &[entity, metadata] : mScene.GetEntities<EntityMetadata>())
        {
            // entity.GetComponent<Transform>().scale = glm::vec3(0.01f);
        }

        mSun = new Light;
        mSun->Initialize();
        mSun->SetPosition(glm::vec3(20.f));
        mSun->SetDirection(glm::vec3(-1, 1, -1));
        mSun->SetIntensity(1);
        mSun->SetColor(glm::vec3(1.0, 0.834, 0.573));
        mSun->SetType(LightType::DirectionalLight);
        mLights.push_back(mSun);
    }

    void OnWindowResize(const glm::uvec2 & /*size*/) override
    {
        Renderer::ResizeSurface(mSurface);
    }

    void OnKeyPress(Key key) override
    {
        if (key == Key::Escape)
        {
            Close();
        }
        if (key == Key::C)
        {
            mLights.clear();
        }
        if (key == Key::L)
        {
            Light *light = new Light;
            light->Initialize();
            light->SetPosition(mCamera.GetPosition());
            light->SetColor(glm::vec3(1, 1, 1));
            light->SetIntensity(20);
            light->SetType(LightType::PointLight);
            light->GenerateShadowMap(Renderer::GetRenderCommands());
            mLights.push_back(light);
        }
    }

    void OnKeyRelease(Key key) override
    {
        if (key == Key::L)
        {
            mLhold = false;
        }

        if (key == Key::Q)
        {
            mToggleLightView = !mToggleLightView;
        }
    }

    bool mLhold = false;

    void OnScroll(const glm::vec2 &scroll) override
    {
    }

    Light *mSun = nullptr;

    bool mToggleLightView = false;

    float mInnerAngle = 50;
    float mOuterAngle = 60;

    void OnUpdate() override
    {
        mController.Update();
        mCamera.Calculate();

        Renderer::BeginLightPlacement();

        glm::mat4 view, projection;

        float speed = 0.5f;

        float t = GetElapsedTime() * speed;

        // mSun->SetDirection(glm::vec3(glm::sin(t), (glm::cos(t) + 1.f) / 2.f, 0));

        for (int i = 0; i < mLights.size(); i++)
        {
            if (mLights[i]->GetType() == LightType::DirectionalLight)
            {

                mLights[i]->SetCamera(mCamera);
            }

            mLights[i]->GenerateShadowMap(Renderer::GetRenderCommands());
            Renderer::AddLight(*mLights[i]);
        }

        Renderer::EndLightPlacement();

        if (mToggleLightView)
        {
            mCamera.SetViewMatrix(view);
            mCamera.SetProjectionMatrix(projection);
        }

        Renderer::BeginFrame(mCamera);

        for (const auto &[entity, component] : mScene.GetEntities<MeshRendererComponent>())
        {
            Renderer::Submit(component.material, component.mesh, entity.GetComponent<Transform>());
        }

        Renderer::EndFrame();

        Renderer::Present(mSurface);
    }

    void OnEnd() override
    {
        MeshManager::Clear();
        MaterialManager::Clear();
        TextureManager::Clear();
        ShaderManager::Clear();
    }
};

CREATE_APPLICATION(RendererRework)
