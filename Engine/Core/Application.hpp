#pragma once
#include "Assets/AssetManager.hpp"
#include "Core/Window.hpp"
#include "Renderer/Renderer.hpp"
#include <memory>

class Application
{
public:
	virtual void Initialize() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void End() {}

	void InitializeApplication();
	void TerminateApplication();
	void RunApplication();

	void Close();
	bool IsRunning();

	bool WindowEventCallback(uint32_t code, void* data);


	std::shared_ptr<AssetManager> GetAssetManager() { return mAssetManager; }
	std::shared_ptr<Renderer> GetRenderer() { return mRenderer; }

	static Application* Create();
	static Application* GetInstance() { return mInstance; }

	Application();
	~Application();

private:
	bool mRunning = true;
	static Application* mInstance;
	void MainLoop();

	std::shared_ptr<Window> mWindow;
	std::shared_ptr<Renderer> mRenderer;

	std::shared_ptr<AssetManager> mAssetManager;

	Material mMaterial;
	Mesh mMesh;
};