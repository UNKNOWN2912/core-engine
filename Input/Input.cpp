#include "Input.hpp"

Mouse::Mouse()
{
    mLeftPressedTimer.Start();
    mRightPressedTimer.Start();
    mMiddlePressedTimer.Start();
}

ButtonState Mouse::GetState(MouseButton button) const
{
    switch (button)
    {
    case MouseButton::None:
        return ButtonState::None;
        break;
    case MouseButton::Left:
        return mLeftState;
        break;
    case MouseButton::Right:
        return mRightState;
        break;
    case MouseButton::Middle:
        return mMiddleState;
        break;
    default:
        break;
    }
}

bool Mouse::IsClicked(MouseButton button, bool immediate) const
{
    if (immediate == false)
    {
        switch (button)
        {
        case MouseButton::None:
            return false;
            break;
        case MouseButton::Left:
            return mLeftClicked;
            break;
        case MouseButton::Right:
            return mRightClicked;
            break;
        case MouseButton::Middle:
            return mMiddleClicked;
            break;
        }
    }
    else
    {
        switch (button)
        {
        case MouseButton::None:
            return false;
            break;
        case MouseButton::Left:
            return mLeftImmediateClicked;
            break;
        case MouseButton::Right:
            return mRightImmediateClicked;
            break;
        case MouseButton::Middle:
            return mMiddleImmediateClicked;
            break;
        }
    }

    return false;
}

bool Mouse::IsDoubleClicked(MouseButton button) const
{
    switch (button)
    {
    case MouseButton::None:
        return false;
        break;
    case MouseButton::Left:
        return mLeftDoubleClicked;
        break;
    case MouseButton::Right:
        return mRightDoubleClicked;
        break;
    case MouseButton::Middle:
        return mMiddleDoubleClicked;
        break;
    }

    return false;
}

bool Mouse::IsDragging(MouseButton button) const
{
    switch (button)
    {
    case MouseButton::None:
        return false;
        break;
    case MouseButton::Left:
        return mIsLeftDragging;
        break;
    case MouseButton::Right:
        return mIsRightDragging;
        break;
    case MouseButton::Middle:
        return mIsMiddleDragging;
        break;
    }

    return false;
}

const glm::vec2 &Mouse::GetPosition() const
{
    return mCursorPosition;
}

const glm::vec2 &Mouse::GetOffset() const
{
    return mCursorOffset;
}

void Mouse::LockCursor(const glm::vec2 &position)
{
    SetCursorPosition(position);
    mLockMouse = true;
}

void Mouse::UnLockCursor()
{
    mLockMouse = false;
}

float Mouse::GetDoubleClickDelay() const
{
    return mDelay;
}

void Mouse::SetDoubleClickDelay(float delay)
{
    mDelay = delay;
}

void Mouse::SetCursorMode(CursorMode mode)
{
    mCursorMode = mode;
}

ButtonState Keyboard::GetState(Key key) const
{
    return mKeyInfoMap.at((int)key).state;
}

float Keyboard::GetDelay() const
{
    return mDelay;
}

void Keyboard::SetDelay(float mDelay_)
{
    mDelay = mDelay_;
}

bool Keyboard::IsClicked(Key key) const
{
    if (mKeyInfoMap[(int)key].clickCounter == 1)
    {
        return true;
    }

    return false;
}

bool Keyboard::IsDoubleClicked(Key key) const
{
    if (mKeyInfoMap[(int)key].clickCounter == 2)
    {
        return true;
    }

    return false;
}