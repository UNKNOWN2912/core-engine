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
    WindowData() = default;
    WindowData(const WindowData &data) = delete;

    WindowData(WindowData &&data) noexcept : window(data.window), dispatcher(std::move(data.dispatcher)), isMaximized(data.isMaximized), preFullscreenData(data.preFullscreenData)
    {
        data.window = nullptr;
    }
    WindowData &operator=(WindowData &&data) noexcept
    {
        window = data.window;
        dispatcher = std::move(data.dispatcher);
        isMaximized = data.isMaximized;
        preFullscreenData = data.preFullscreenData;

        data.window = nullptr;
        return *this;
    }
    GLFWwindow *window = nullptr;
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
    Window(const glm::uvec2 &size, std::string_view title);
    Window() = default;
    Window(const Window &) = default;
    Window(Window &&window) noexcept
    {
        mWindowData.window = window.mWindowData.window;
        mWindowData.dispatcher = std::move(window.mWindowData.dispatcher);
        mWindowData.isMaximized = window.mWindowData.isMaximized;
        mWindowData.preFullscreenData = window.mWindowData.preFullscreenData;

        window.mWindowData = {};
        glfwSetWindowUserPointer(mWindowData.window, &mWindowData);
    }
    Window &operator=(const Window &) = default;
    Window &operator=(Window &&window) noexcept
    {
        mWindowData.window = window.mWindowData.window;
        mWindowData.dispatcher = std::move(window.mWindowData.dispatcher);
        mWindowData.isMaximized = window.mWindowData.isMaximized;
        mWindowData.preFullscreenData = window.mWindowData.preFullscreenData;

        window.mWindowData = {};
        glfwSetWindowUserPointer(mWindowData.window, &mWindowData);
        return *this;
    }
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

private:
    WindowData mWindowData;
};
