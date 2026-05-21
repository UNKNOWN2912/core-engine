#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <Core/Event.hpp>
#include <glm/glm.hpp>
#include <string>

enum class WindowEvent
{
	WindowClose,
	WindowResize,
	WindowMove,
	WindowMinimize,
	WindowMaximize,

	WindowMouseMove,
	WindowMousePress,
	WindowMouseRelease,
	WindowScroll,

	WindowKeyPress,
	WindowKeyRelease,
	WindowKeyRepeat,

	WindowCharacterType
};

struct WindowData
{
	GLFWwindow* window = nullptr;
	EventDispatcher dispatcher;
	bool isMaximized = false;
	struct PreFullscreenData 
	{
		int width = 0;
		int height = 0;
		int x = 0;
		int y = 0;
	} preFullscreenData;
};

class Window
{
public:
	void CreateWindow(const glm::uvec2& size, std::string_view title);
	void DestroyWindow();

	glm::uvec2 GetSize() const;
	glm::uvec2 GetPosition() const;
	glm::uvec2 GetFrameBufferSize() const;
	std::string GetTitle() const;

	void SetSize(const glm::uvec2& size);
	void SetPosition(const glm::uvec2& position);
	void SetTitle(const std::string& title);
	void AddListener(std::function<bool(uint32_t code, void* data)> listener);
	void ProcessEvent();
	GLFWwindow* GetNativeWindow() const;

	bool isFullscreen();
	void SetFullscreen(bool fullscreen);

	void HideCursor();
	void ShowCursor();
	bool isCursorHidden();

	void Maximize();
	void Restore();
	bool IsMaximized();

	Window() 
	{

	}

	~Window();
private:
	WindowData mWindowData;
};




