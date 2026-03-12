#pragma once
#include "Assets/AssetManager.hpp"
#include "Renderer/Vulkan/VulkanGraphicPipeline.hpp"
#include "Renderer/Vulkan/VulkanSwapchain.hpp"
#include "Renderer/Vulkan/VertexBuffer.hpp"
#include "Renderer/Vulkan/IndexBuffer.hpp"
#include <Renderer/Renderer.hpp>
#include <vulkan/vulkan.h>

struct QueueIndex
{
	uint32_t graphic = UINT32_MAX;
	uint32_t transfer = UINT32_MAX;
	uint32_t present = UINT32_MAX;
	uint32_t compute = UINT32_MAX;
};

struct Queue
{
	VkQueue graphic = VK_NULL_HANDLE;
	VkQueue transfer = VK_NULL_HANDLE;
	VkQueue present = VK_NULL_HANDLE;
	VkQueue compute = VK_NULL_HANDLE;
};

struct FrameData
{
	VkFramebuffer frameBuffer = VK_NULL_HANDLE;
};

struct VulkanShader
{
	VkShaderModule mVertexShader;
	VkShaderModule mFragmentShader;
};

struct VulkanMeshRenderer
{
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
	VkPipeline pipeline;
};


class VulkanRenderer : public Renderer
{
	
public:
	VulkanRenderer(const Window& window);

	void Initialize(const Window& window) override;
	void BeginFrame(const Vector4f& clearColor) override;
	void EndFrame() override;
	void Submit(std::shared_ptr<MeshRenderer> meshRenderer) override;
    void CreateAssets(std::shared_ptr<AssetManager> assetManager) override;
	void CreateMeshRendererObjects(std::shared_ptr<MeshRenderer> meshRenderer) override;
	
	void CreatePipelineLayout();

	void CreateVulkanObjects(const Window& window);
	void CreateInstance();
	void GetPhysicalDevice();
	void CreateDevice();
	void GetQueueIndices();
	void CreateQueues();
	void CreateSurface(const Window& window);
	void CreateSwapchain(const Window& window);
	void CreateMainRenderPass();
	void CreateMainRenderPassFrameBuffer();
	void CreateRenderCommandBuffer();
	void CreateCommandPool();
	void CreateMeshRendererPipeline(std::shared_ptr<MeshRenderer> meshRenderer);



private:
	VkClearValue mClearValue;
	VkInstance mInstance = VK_NULL_HANDLE;
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;
	VkCommandPool mCommandPool = VK_NULL_HANDLE;
	VulkanSwapchain mSwapchain;
	QueueIndex mQueueIndices;
	Queue mQueues;
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	VkRenderPass mMainRenderPass = VK_NULL_HANDLE;
	VkViewport mViewport = {};
	VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	std::vector<FrameData> mFrameData;

	VkSemaphore mImageAcquiredSemaphore = VK_NULL_HANDLE;
	VkSemaphore mRenderFinished = VK_NULL_HANDLE;

	std::unordered_map<std::string, VulkanShader> mShaders;
	std::unordered_map<std::shared_ptr<MeshRenderer>, std::shared_ptr<VulkanGraphicPipeline>> mMeshRendererPipeline;

	std::vector<std::shared_ptr<MeshRenderer>> mMeshRenderers;

	VkPipelineLayout mPipelineLayout;
};