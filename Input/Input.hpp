#pragma once
#include "Core/Timer.hpp"
#include "Core/Window.hpp"
#include "Input/Keyboard.hpp"
#include "Mouse.hpp"
#include <glm/glm.hpp>

enum class ButtonState
{
    None,
    Pressed,
    Released,
    Repeat,
};

enum class CursorMode
{
    None,
    Normal,
    Disable,
    Hidden
};

class Mouse
{
public:
    Mouse();

    ButtonState GetState(MouseButton button) const;
    bool IsClicked(MouseButton button, bool immediateMode = false) const;
    bool IsDoubleClicked(MouseButton button) const;
    bool IsDragging(MouseButton button) const;
    float GetDoubleClickDelay() const;
    const glm::vec2 &GetPosition() const;
    const glm::vec2 &GetOffset() const;

    void LockCursor(const glm::vec2 &position);
    void UnLockCursor();

    void SetDoubleClickDelay(float delay);
    void ProcessMouseEvent(const Window &window);
    void SetCursorMode(CursorMode mode);

    void SetCursorPosition(const glm::vec2 &position)
    {
        mNewCursorPosition = position;
        mSetCursorPosition = true;
    }

private:
    float mDelay = 0.2f;

    bool mLockMouse = false;

    bool mLeftClicked = false;
    bool mRightClicked = false;
    bool mMiddleClicked = false;

    bool mLeftImmediateClicked = false;
    bool mRightImmediateClicked = false;
    bool mMiddleImmediateClicked = false;

    bool mLeftDoubleClicked = false;
    bool mRightDoubleClicked = false;
    bool mMiddleDoubleClicked = false;

    bool mSetCursorPosition = false;

    bool mIsLeftDragging = false;
    bool mIsRightDragging = false;
    bool mIsMiddleDragging = false;

    uint32_t mLeftClickCounter = 0;
    uint32_t mRightClickCounter = 0;
    uint32_t mMiddleClickCounter = 0;

    ButtonState mLeftState = ButtonState::None;
    ButtonState mRightState = ButtonState::None;
    ButtonState mMiddleState = ButtonState::None;

    Timer mLeftPressedTimer;
    Timer mRightPressedTimer;
    Timer mMiddlePressedTimer;

    glm::vec2 mNewCursorPosition = glm::vec2(0);

    CursorMode mCursorMode = CursorMode::Normal;
    glm::vec2 mCursorPosition = glm::vec2(0);
    glm::vec2 mCursorOffset = glm::vec2(0);

    friend class EditorUI;
};

struct KeyInfo
{
    Timer clickTimer;
    ButtonState state = ButtonState::None;
    uint32_t clickCounter = 0;

    KeyInfo()
    {
        clickTimer.Start();
    }
};

class Keyboard
{
public:
    ButtonState GetState(Key key) const;
    float GetDelay() const;
    void SetDelay(float mDelay_);

    bool IsClicked(Key key) const;
    bool IsDoubleClicked(Key key) const;

    void ProcessKeyboardEvent(const Window &window);

private:
    std::array<KeyInfo, (int)Key::MaxEnum> mKeyInfoMap;
    float mDelay = 0.2f;
};