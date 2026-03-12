#pragma once
#include "Assets/AssetManager.hpp"
#include <memory>
#include <Renderer/MeshRenderer.hpp>
#include <Renderer/GraphicContext.hpp>

class Renderer
{
public:
	virtual void Initialize(const Window& window) = 0;

	virtual void BeginFrame(const Vector4f& clearColor) = 0;
	virtual void EndFrame() = 0;

	virtual void Submit(std::shared_ptr<MeshRenderer> meshRenderer) = 0;

	virtual void CreateAssets(std::shared_ptr<AssetManager> assetManager) = 0;

	virtual void CreateMeshRendererObjects(std::shared_ptr<MeshRenderer> meshRenderer) = 0;

	static Renderer* Create(const Window& window);
private:

	std::shared_ptr<GraphicContext> mContext;
};