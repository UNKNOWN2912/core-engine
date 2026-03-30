#pragma once
#include "Core/Window.hpp"
#include "Renderer/Renderer.hpp"

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


	Renderer& GetRendererRef() { return mRenderer; }

	static Application* Create();
	static Application* GetInstance() { return mInstance; }

	Application();
	virtual ~Application();
private:
	bool mRunning = true;
	static Application* mInstance;
	void MainLoop();

	Window mWindow;
	Renderer mRenderer;
};