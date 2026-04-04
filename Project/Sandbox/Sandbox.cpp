#include <Engine.hpp>



class Sandbox : public Application
{
	public:
		Sandbox(): mGraphic(GetRendererRef().GetGraphicRef())
		{
		}
		void Start() override 
		{
			
			Attachment attachment = 
			{
				.usage = ImageUsage::Present,
				.loadOperation = LoadOperation::Clear,
				.storeOperation = StoreOperation::Store,
				.clearValue = glm::vec4(0.1f),
				.format = ImageFormat::BGRA8
			};

			Subpass subpass;
			subpass.AddColorAttachment(0);

			Dependency dependency;
			dependency.SetSourceSubpass(UINT32_MAX);
			dependency.SetDestinationSubpass(0);

			mRenderPass.AddAttachment(attachment);
			mRenderPass.AddSubpass(subpass, dependency);

			mGraphic.CreateRenderPass(mRenderPass);
			mGraphic.CreateCommandBuffer(mCommandBuffer);

			mViewport.size = GetWindowRef().GetSize();

			mGraphic.CreateSwapchain(mSwapchain);

			for (size_t i = 0; i < mSwapchain.GetImages().size(); i++)
			{
				FrameBuffer frameBuffer;
				mGraphic.CreateFrameBufferWithUserAttachments(frameBuffer, mRenderPass, mViewport, {mSwapchain.GetImages()[i]});
				mFrameBuffer.push_back(frameBuffer);
			}

			mImageAcquiredSemaphore = mGraphic.CreateSemaphore();
		}
		void Update() override 
		{
			uint32_t index = mGraphic.GetNextSwapchainImage(mSwapchain, mImageAcquiredSemaphore);
			mGraphic.BeginCommandBufferRecording(mCommandBuffer);
			
			mGraphic.BeginRenderPass(mCommandBuffer, mRenderPass, mFrameBuffer[index], mViewport);
			
			mGraphic.BindVertexBuffer(mVertexBuffer, 0);
			mGraphic.BindIndexBuffer(mIndexBuffer);
			mGraphic.DrawIndexed(3);
			
			mGraphic.EndRenderPass(mCommandBuffer);
			
			mGraphic.EndCommandBufferRecording(mCommandBuffer);
			
			mGraphic.ExecuteCommandBuffer(mCommandBuffer, QueueType::Graphic);
			
			mGraphic.PresentSwapchainImage(mSwapchain, index, mImageAcquiredSemaphore);
		}
		void End() override 
		{
		}
	private:
		Graphic& mGraphic;
		DeviceSemaphore mImageAcquiredSemaphore;
		VertexBuffer mVertexBuffer;
		IndexBuffer mIndexBuffer;
		RenderPass mRenderPass;
		CommandBuffer mCommandBuffer;
		Viewport mViewport = {};
		std::vector<FrameBuffer> mFrameBuffer;
		Swapchain mSwapchain;
};

Application *Application::Create() { return new Sandbox(); }