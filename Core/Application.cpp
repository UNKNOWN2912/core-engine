#include "Application.hpp"
#include "Input/Mouse.hpp"
#include <cassert>

void Application::InitializeApplication()
{
    CHROME_TRACE_FUNCTION();

    OnInitialize();

    mApplicationTimer.Start();
    mWindow = Window({800, 600}, "Untitled");
    mWindow.AddListener(BindMember(Application::WindowEventCallback));

    Renderer::Initialize(mRendererSpecification);
}

void Application::TerminateApplication()
{
    CHROME_TRACE_FUNCTION();
    Renderer::Terminate();
}

void Application::RunApplication()
{
    CHROME_TRACE_FUNCTION();
    InitializeApplication();
    OnStart();
    MainLoop();
    OnEnd();
    TerminateApplication();
}

void Application::Close()
{
    CHROME_TRACE_FUNCTION();
    mRunning = false;
}

bool Application::IsRunning()
{
    CHROME_TRACE_FUNCTION();
    return mRunning;
}

bool Application::WindowEventCallback(uint32_t code, void *data)
{
    CHROME_TRACE_FUNCTION();
    WindowEvent event = (WindowEvent)code;

    switch (event)
    {
    case WindowEvent::WindowClose: {
        OnWindowClose();
        break;
    }
    case WindowEvent::WindowResize: {
        glm::uvec2 size = *(glm::uvec2 *)data;
        OnWindowResize(size);
        break;
    }
    case WindowEvent::WindowMove: {
        glm::uvec2 position = *(glm::uvec2 *)data;
        mCursorPosition = position;
        OnWindowMove(position);
        break;
    }
    case WindowEvent::WindowMousePress: {
        MouseButton button = *(MouseButton *)data;
        OnMouseButtonPress(button);
        break;
    }
    case WindowEvent::WindowMouseRelease: {
        MouseButton button = *(MouseButton *)data;
        OnMouseButtonRelease(button);
        break;
    }
    case WindowEvent::WindowMinimize: {
        OnWindowMinimize();
        break;
    }
    case WindowEvent::WindowMaximize: {
        OnWindowMaximize();
        break;
    }
    case WindowEvent::WindowMouseMove: {
        glm::vec2 position = *(glm::vec2 *)data;
        glm::vec2 offset = position - previousMousePos;
        previousMousePos = position;
        mCursorPosition = position;
        OnMouseMove(position, offset);
        break;
    }
    case WindowEvent::WindowScroll: {
        glm::vec2 scroll = *(glm::vec2 *)data;
        OnScroll(scroll);
        break;
    }
    case WindowEvent::WindowKeyPress: {
        Key key = *(Key *)data;
        OnKeyPress(key);
        break;
    }
    case WindowEvent::WindowKeyRepeat: {
        Key key = *(Key *)data;
        OnKeyRepeat(key);
        break;
    }
    case WindowEvent::WindowKeyRelease: {
        Key key = *(Key *)data;
        OnKeyRelease(key);
        break;
    }
    case WindowEvent::WindowCharacterType: {
        char ch = *(char *)data;
        OnCharacterType(ch);
        break;
    }
    }

    mLayerStack.InvokeEvents(code, data);

    return false;
}

Window &Application::GetWindow()
{
    return mWindow;
}

Application *Application::GetInstance()
{
    return instance;
}

Application::Application()
{
    CHROME_TRACE_FUNCTION();
    assert(instance == nullptr);

    instance = this;
}

Application::~Application()
{
    CHROME_TRACE_FUNCTION();
}

float Application::GetDeltaTime()
{
    return mDeltaTime;
}

float Application::GetElapsedTime()
{
    return mApplicationTimer.GetElapsedTime();
}

uint32_t Application::GetFrameCount() const
{
    return mFrameCounter;
}
uint32_t Application::GetFps() const
{
    return mFps;
}
glm::vec2 Application::GetCursorPos() const
{
    return mWindow.GetCursorPosition();
}

void Application::MainLoop()
{
    CHROME_TRACE_FUNCTION();

    Timer fpsTimer(true);
    uint32_t fps = 0;

    while (mRunning)
    {
        mWindow.ProcessEvent();

        if (mDisableCursor)
        {
            mWindow.DisableCursor();
        }
        else if (mHideCursor)
        {
            mWindow.HideCursor();
        }
        else
        {
            mWindow.ShowCursor();
        }

        fps++;
        if (fpsTimer.GetElapsedTime() > 1.f)
        {
            mFps = fps;
            fps = 0;
            fpsTimer.Start();
        }
        mFrameCounter++;
        mDeltaTimer.Start();
        OnUpdate();
        mLayerStack.InvokeUpdates();
        mDeltaTimer.Stop();
        mDeltaTime = mDeltaTimer.GetDuration();
    }
}

void Application::HideCursor()
{
    CHROME_TRACE_FUNCTION();
    mHideCursor = true;
}

void Application::ResetCursor()
{
    mDisableCursor = false;
    mHideCursor = false;
}

void Application::DisableCursor()
{
    mDisableCursor = true;
}

bool Application::IsCursorHidden()
{
    CHROME_TRACE_FUNCTION();
    return mWindow.isCursorHidden();
}

Application *Application::instance = nullptr;