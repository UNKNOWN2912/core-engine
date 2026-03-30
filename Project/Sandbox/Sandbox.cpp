#include <Engine.hpp>



class Sandbox : public Application
{
	public:
		Sandbox(): mGraphic(GetRendererRef().GetGraphicRef())
		{
		}
		void Start() override 
		{
		}
		void Update() override 
		{
			mGraphic.BeginCommandBufferRecording(mCommandBuffer);

			mGraphic.BeginRenderPass(mRenderPass);

			mGraphic.BindVertexBuffer(mVertexBuffer, 0);
			mGraphic.BindIndexBuffer(mIndexBuffer);
			mGraphic.DrawIndexed(3);

			mGraphic.EndRenderPass();

			mGraphic.EndCommandBufferRecording(mCommandBuffer);


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
};

Application *Application::Create() { return new Sandbox(); }