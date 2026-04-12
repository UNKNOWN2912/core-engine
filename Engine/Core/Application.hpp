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

	virtual void OnWindowMove(const glm::uvec2& position) {}
	virtual void OnWindowResize(const glm::uvec2& size) {}

	void InitializeApplication();
	void TerminateApplication();
	void RunApplication();

	void Close();
	bool IsRunning();

	bool WindowEventCallback(uint32_t code, void* data);


	Window& GetWindowRef() { return mWindow; }

	static Application* Create();
	static Application* GetInstance() { return instance; }

	Application();
	virtual ~Application();

	Renderer mRenderer;
private:
	bool mRunning = true;
	static Application* instance;
	void MainLoop();

	Window mWindow;
};