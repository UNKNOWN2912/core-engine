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


class MaterialMap
{
    public:
        void Set(const std::string& identifier, std::shared_ptr<Material> material)
        {
            mMap[identifier] = material;
        }
        void Remove(const std::string& identifier)
        {
            mMap.erase(identifier);
        }

        std::shared_ptr<Material> Get(const std::string& identifier)
        {
            return mMap[identifier];
        }

        const std::unordered_map<std::string, std::shared_ptr<Material>>& GetMap()
        {
            return mMap;
        }

        bool Has(const std::string& identifier)
        {
            return mMap.contains(identifier);
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Material>> mMap;
};

class TextureMap
{
    public:
        void Set(const std::string& identifier, std::shared_ptr<Texture> texture)
        {
            mMap[identifier] = texture;
        }
        void Remove(const std::string& identifier)
        {
            mMap.erase(identifier);
        }

        std::shared_ptr<Texture> Get(const std::string& identifier)
        {
            return mMap[identifier];
        }

        const std::unordered_map<std::string, std::shared_ptr<Texture>>& GetMap()
        {
            return mMap;
        }

        bool Has(const std::string& identifier)
        {
            return mMap.contains(identifier);
        }
    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> mMap;
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
        
        MaterialMap materialMap;
        TextureMap textureMap;

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

        CommandBuffer commandBuffer;


        StaticMesh mesh;


        std::shared_ptr<StaticMesh> GetMeshFromAiMesh(const aiMesh* mesh);

        std::shared_ptr<Material> GetMaterialFromAiMaterial(const aiScene* scene, const aiMaterial* aimaterial, std::string_view basePath);

        void LoadNode(Scene& scene, const aiScene* aiscene, aiNode* node, std::string_view basePath);

        std::vector<Entity> frontRightTier;
        std::vector<Entity> frontLeftTier;
        std::vector<Entity> backRightTier;
        std::vector<Entity> backLeftTier;
};