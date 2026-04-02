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
				.usage = ImageUsage::ColorOutput,
				.loadOperation = LoadOperation::DontCare,
				.storeOperation = StoreOperation::Store,
				.clearValue = glm::vec4(1.f),
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

			mGraphic.CreateFrameBuffer(mFrameBuffer, mRenderPass, mViewport);

		}
		void Update() override 
		{
			mGraphic.BeginCommandBufferRecording(mCommandBuffer);

			mGraphic.BeginRenderPass(mCommandBuffer, mRenderPass, mFrameBuffer, mViewport);

			mGraphic.BindVertexBuffer(mVertexBuffer, 0);
			mGraphic.BindIndexBuffer(mIndexBuffer);
			mGraphic.DrawIndexed(3);

			mGraphic.EndRenderPass(mCommandBuffer);

			mGraphic.EndCommandBufferRecording(mCommandBuffer);

			mGraphic.ExecuteCommandBuffer(mCommandBuffer, QueueType::Graphic);

			mGraphic.WaitForDevice();
		}
		void End() override 
		{
		}
	private:
		Graphic& mGraphic;

		VertexBuffer mVertexBuffer;
		IndexBuffer mIndexBuffer;
		RenderPass mRenderPass;
		CommandBuffer mCommandBuffer;
		Viewport mViewport = {};
		FrameBuffer mFrameBuffer;
		Swapchain mSwapchain;
};

Application *Application::Create() { return new Sandbox(); }