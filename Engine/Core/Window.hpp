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
    GLFWwindow *window = nullptr;
    EventDispatcher dispatcher;
    bool isMaximized = false;
    struct RestoreData
    {
        int width = 0;
        int height = 0;
        int x = 0;
        int y = 0;
    } restoreData;
};

class Window
{
public:
    Window(const glm::uvec2 &size, std::string_view title);
    Window() = default;
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&window) noexcept;
    Window &operator=(Window &&window) noexcept;
    ~Window();

    glm::uvec2 GetSize() const;
    glm::uvec2 GetPosition() const;
    glm::uvec2 GetFrameBufferSize() const;
    std::string GetTitle() const;

    void DestroyWindow();

    void SetSize(const glm::uvec2 &size);
    void SetPosition(const glm::uvec2 &position);
    void SetTitle(const std::string &title);
    void AddListener(const std::function<bool(uint32_t code, void *data)> &listener);
    void ProcessEvent();
    GLFWwindow *GetNativeWindow() const;

    bool IsFullscreen() const;
    void SetFullscreen(bool fullscreen);

    void HideCursor();
    void ShowCursor();
    bool isCursorHidden() const;

    void Maximize();
    void Restore();
    bool IsMaximized() const;

    VkSurfaceKHR CreateWindowSurface() const;

private:
    WindowData mWindowData;
};
