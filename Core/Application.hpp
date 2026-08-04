#pragma once
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Assets/ShaderManager.hpp"
#include "Assets/TextureManager.hpp"
#include "Core/LayerStack.hpp"
#include "Core/Window.hpp"
#include "Input/Keyboard.hpp"
#include "Input/Mouse.hpp"
#include "Renderer/Renderer.hpp"

class Application
{
public:
    virtual void OnInitialize()
    {
    }
    virtual void OnStart()
    {
    }
    virtual void OnUpdate()
    {
    }
    virtual void OnEnd()
    {
    }

    virtual void OnWindowClose()
    {
        Close();
    }
    virtual void OnWindowMove(const glm::uvec2 &position)
    {
    }
    virtual void OnWindowResize(const glm::uvec2 &size)
    {
    }
    virtual void OnWindowMinimize()
    {
    }
    virtual void OnWindowMaximize()
    {
    }

    virtual void OnMouseMove(const glm::vec2 &position, const glm::vec2 &offset)
    {
    }
    virtual void OnMouseButtonPress(MouseButton button)
    {
    }
    virtual void OnMouseButtonRelease(MouseButton button)
    {
    }
    virtual void OnScroll(const glm::vec2 &scroll)
    {
    }

    virtual void OnKeyPress(Key key)
    {
    }
    virtual void OnKeyRepeat(Key key)
    {
    }
    virtual void OnKeyRelease(Key key)
    {
    }

    virtual void OnCharacterType(char ch)
    {
    }

    void InitializeApplication();
    void TerminateApplication();
    void RunApplication();

    void DisableCursor();
    void HideCursor();
    void ResetCursor();
    bool IsCursorHidden();

    void Close();
    bool IsRunning();

    bool WindowEventCallback(uint32_t code, void *data);

    Window &GetWindow()
    {
        return mWindow;
    }

    static Application *Create();
    static Application *GetInstance()
    {
        return instance;
    }

    Application();
    virtual ~Application();

    float GetDeltaTime();
    float GetElapsedTime();

    template <typename T, typename... Args>
        requires std::derived_from<T, Layer>
    void AttachLayer(Args... args)
    {
        mLayerStack.Attach<T>(args...);
    }

    template <typename T>
        requires std::derived_from<T, Layer>
    void DetachLayer()
    {
        mLayerStack.Detach<T>();
    }

    template <typename T>
        requires std::derived_from<T, Layer>
    const T &GetLayer() const
    {
        return mLayerStack.Get<T>();
    }

    template <typename T>
        requires std::derived_from<T, Layer>
    T &GetLayer()
    {
        return mLayerStack.Get<T>();
    }

    uint32_t GetFrameCount() const;
    uint32_t GetFps() const;

    glm::vec2 GetCursorPos() const;

    const RendererSpecification &GetRendererSpecification() const
    {
        return mRendererSpecification;
    }
    void SetRendererSpecification(const RendererSpecification &specification)
    {
        mRendererSpecification = specification;
    }

private:
    bool mRunning = true;
    static Application *instance;
    void MainLoop();

    glm::vec2 previousMousePos = glm::vec2(0);
    Window mWindow;

    glm::uvec2 mCursorPosition = glm::uvec2(0);

    uint32_t mFps = 0;

    Timer mDeltaTimer;
    Timer mApplicationTimer;

    float mDeltaTime = 0;

    LayerStack mLayerStack;

    uint32_t mFrameCounter = 0;

    bool mHideCursor = false;
    bool mDisableCursor = false;

    RendererSpecification mRendererSpecification;
};
