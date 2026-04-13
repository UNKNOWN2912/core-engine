#pragma once
#include "Core/Window.hpp"
#include "Input/Keyboard.hpp"
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
	virtual void OnMouseMove(const glm::vec2& position, const glm::vec2& offset) {}
	virtual void OnScroll(const glm::vec2& scroll) {}

	virtual void OnKeyPress(Key key) {}
	virtual void OnKeyRepeat(Key key) {}
	virtual void OnKeyRelease(Key key) {}

	virtual void OnCharacterType(char ch) {}

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

	glm::vec2 previousMousePos = glm::vec2(0);

	Window mWindow;
};