#pragma once
#include "Core/LayerStack.hpp"
#include "Core/Window.hpp"
#include "Input/Keyboard.hpp"
#include "Input/Mouse.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/Macro.hpp"


class Application
{
public:
	virtual void Initialize() {}
	virtual void OnStart() {}
	virtual void OnUpdate() {}
	virtual void OnEnd() {}

	virtual void OnWindowClose() { Close(); }
	virtual void OnWindowMove(const glm::uvec2& position) {}
	virtual void OnWindowResize(const glm::uvec2& size) {}
	virtual void OnWindowMinimize() {}
	virtual void OnWindowMaximize() {}
	
	virtual void OnMouseMove(const glm::vec2& position, const glm::vec2& offset) {}
	virtual void OnMouseButtonPress(MouseButton button) {}
	virtual void OnMouseButtonRelease(MouseButton button) {}
	virtual void OnScroll(const glm::vec2& scroll) {}

	virtual void OnKeyPress(Key key) {}
	virtual void OnKeyRepeat(Key key) {}
	virtual void OnKeyRelease(Key key) {}

	virtual void OnCharacterType(char ch) {}

	void InitializeApplication();
	void TerminateApplication();
	void RunApplication();

	void HideCursor();
	void ShowCursor();
	void ToggleCursor();
	bool IsCursorHidden();

	void Close();
	bool IsRunning();

	bool WindowEventCallback(uint32_t code, void* data);

	Window& GetWindowRef() { return mWindow; }
	Renderer& GetRendererRef() { return mRenderer; }

	static Application* Create();
	static Application* GetInstance() { return instance; }

	Application();
	virtual ~Application();

	Renderer mRenderer;

	float GetDeltaTime();
	float GetElapsedTime();

	template<typename T, typename ...Args> requires std::derived_from<T, Layer>
	void AttachLayer(Args... args)
	{
		mLayerStack.Attach<T>(args...);
	}

	template<typename T> requires std::derived_from<T, Layer>
	void DetachLayer()
	{
		mLayerStack.Detach<T>();
	}

	template<typename T> requires std::derived_from<T, Layer>
	const T& GetLayer() const 
	{
		return mLayerStack.Get<T>();
	}

	template<typename T> requires std::derived_from<T, Layer>
	T& GetLayer() 
	{
		return mLayerStack.Get<T>();
	}

private:
	bool mRunning = true;
	static Application* instance;
	void MainLoop();

	glm::vec2 previousMousePos = glm::vec2(0);
	Window mWindow;

	Timer mDeltaTimer;
	Timer mApplicationTimer;

	float mDeltaTime = 0;

	LayerStack mLayerStack;
};