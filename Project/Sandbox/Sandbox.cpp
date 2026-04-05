#include <Engine.hpp>



class Sandbox : public Application
{
	public:
		Sandbox(): mGraphic(GetRendererRef().GetGraphicRef())
		{
		}
		void Start() override 
		{
			
			Attachment attachment;
			attachment.SetFormat(ImageFormat::BGRA8)
					   .SetLoadOperation(LoadOperation::Clear)
					   .SetStoreOperation(StoreOperation::Store)
					   .SetUsage(ImageUsage::ColorOutput)
					   .SetClearValue(glm::vec4(1.f));

			Subpass subpass;
			subpass.AddColorAttachment(0);

			Dependency dependency;
			dependency.SetSourceSubpass(UINT32_MAX)
			           .SetDestinationSubpass(0);

			mRenderPass.AddAttachment(attachment)
				 		.AddSubpass(subpass, dependency);

			mGraphic.CreateRenderPass(mRenderPass);
						 
			mViewport.SetSize(GetWindowRef().GetSize())
					  .SetPosition(glm::uvec2(0));
			
			mGraphic.CreateSwapchain(mSwapchain);
			
			for (size_t i = 0; i < mSwapchain.GetImages().size(); i++)
			{
				FrameBuffer frameBuffer;
				mGraphic.CreateFrameBufferWithUserAttachments(frameBuffer, mRenderPass, mViewport, {mSwapchain.GetImages()[i]});
				mFrameBuffer.push_back(frameBuffer);
			}
			
			mGraphic.CreateSemaphore(mImageAcquiredSemaphore);
			mGraphic.CreateSemaphore(mRenderFinishedSemaphore);

			mGraphic.CreateCommandBuffer(mCommandBuffer);

			mVertexShader.SetType(ShaderType::Vertex);
			mFragmentShader.SetType(ShaderType::Fragment);

			mGraphic.CreateShaderFromFile(mVertexShader, "Shader/shader.vert.spv");
			mGraphic.CreateShaderFromFile(mFragmentShader, "Shader/shader.frag.spv");

			mGraphic.CreatePipelineLayout(mPipelineLayout);

			mGraphicPipelineShader.SetVertexShader(mVertexShader)
								  .SetFragmentShader(mFragmentShader)
								  .SetPipelineLayout(mPipelineLayout);

			VertexLayout vertexLayout;
			vertexLayout.AddBinding(0, sizeof(glm::vec3), false)
						.AddAttribute(0, 0, 0, VertexFormatType::Vec3);

			mGraphicPipeline.SetShader(mGraphicPipelineShader)
							.SetVertexLayout(vertexLayout);
			
			mGraphic.CreateGraphicPipeline(mGraphicPipeline, mRenderPass, 0, mViewport);
			
		}
		void Update() override 
		{
			mGraphic.WaitForDevice();
			uint32_t index = mGraphic.GetNextSwapchainImage(mSwapchain, mImageAcquiredSemaphore);
			mGraphic.BeginCommandBufferRecording(mCommandBuffer);
			
			mGraphic.BeginRenderPass(mCommandBuffer, mRenderPass, mFrameBuffer[index], mViewport);
			
			mGraphic.BindVertexBuffer(mVertexBuffer, 0);
			mGraphic.BindIndexBuffer(mIndexBuffer);
			mGraphic.DrawIndexed(3);
			
			mGraphic.EndRenderPass(mCommandBuffer);
			
			mGraphic.EndCommandBufferRecording(mCommandBuffer);
			
			mGraphic.ExecuteCommandBuffer(mCommandBuffer, QueueType::Graphic, {mImageAcquiredSemaphore}, mRenderFinishedSemaphore);
			
			mGraphic.PresentSwapchainImage(mSwapchain, index, mRenderFinishedSemaphore);


		}
		void End() override 
		{
		}
	private:
		GraphicPipeline mGraphicPipeline;
		GraphicPipelineShader mGraphicPipelineShader;
		PipelineLayout mPipelineLayout;

		Graphic& mGraphic;
		DeviceSemaphore mImageAcquiredSemaphore;
		DeviceSemaphore mRenderFinishedSemaphore;
		VertexBuffer mVertexBuffer;
		IndexBuffer mIndexBuffer;
		RenderPass mRenderPass;
		CommandBuffer mCommandBuffer;
		Viewport mViewport = {};
		std::vector<FrameBuffer> mFrameBuffer;
		Swapchain mSwapchain;

		Shader mVertexShader;
		Shader mFragmentShader;
};

Application *Application::Create() { return new Sandbox(); }