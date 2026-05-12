#pragma once
#include "CameraController.hpp"
#include "Core/LayerStack.hpp"
#include "EntityComponentSystem.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Transform.hpp"
#include "assimp/scene.h"

struct UniformData
{
    glm::vec3 lightPosition = glm::vec3(0);
    float intensity = 0;
};

class GameLayer : public Layer
{
    public:
        void OnAttach() override;
        void OnUpdate() override;
        void OnDetach() override;

        void UpdateCamera();

        void DestroyMaterial();

        void CreateMaterial();

        void ReloadMaterial();

        Scene scene;


        std::vector<std::shared_ptr<StaticMesh>> mModel;
        std::vector<std::shared_ptr<Material>> mMaterial;
        std::vector<Transform> mTransforms;

        std::vector<std::pair<uint32_t, uint32_t>> mMeshMaterialPair;

        Camera mCamera;
        CameraController mCameraController;

        RenderTarget mTarget;
        
        CommandBuffer mImGuiCommandBuffer;
        RenderPass mImguiRenderPass;
        Semaphore mImageAcquiredSemaphore;
        Semaphore mRenderingFinished;


        Material skyboxMaterial;
        Material material;

        CommandBuffer commandBuffer;


        StaticMesh mesh;


        std::shared_ptr<StaticMesh> GetMeshFromAiMesh(const aiMesh* mesh);

        std::shared_ptr<Material> GetMaterialFromAiMaterial(const aiMaterial* aimaterial, std::string_view basePath);

        void LoadNode(Scene& scene, const aiScene* aiscene, aiNode* node, std::string_view basePath);

};