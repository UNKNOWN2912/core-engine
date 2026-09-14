#include "Input/Input.hpp"
#include <GLFW/glfw3.h>

ButtonState GetNativeButtonState(int glfwState)
{
    switch (glfwState)
    {
    case GLFW_PRESS:
        return ButtonState::Pressed;
        break;
    case GLFW_RELEASE:
        return ButtonState::Released;
        break;
    case GLFW_REPEAT:
        return ButtonState::Repeat;
        break;
    }

    return ButtonState::None;
}

CursorMode GetNativeCursorMode(int glfwCursorMode)
{
    switch (glfwCursorMode)
    {
    case GLFW_CURSOR_HIDDEN:
        return CursorMode::Hidden;
        break;
    case GLFW_CURSOR_DISABLED:
        return CursorMode::Disable;
    case GLFW_CURSOR_NORMAL:
        return CursorMode::Normal;
    }

    return CursorMode::None;
}

int GetGlfwCursorMode(CursorMode mode)
{
    switch (mode)
    {
    case CursorMode::None:
        break;
    case CursorMode::Normal:
        return GLFW_CURSOR_NORMAL;
        break;
    case CursorMode::Disable:
        return GLFW_CURSOR_DISABLED;
        break;
    case CursorMode::Hidden:
        return GLFW_CURSOR_HIDDEN;
        break;
    }

    return 0;
}

void Mouse::ProcessMouseEvent(const Window &window)
{
    GLFWwindow *glfwWindow = window.GetNativeWindow();

    mLeftClicked = false;
    mRightClicked = false;
    mMiddleClicked = false;
    mRightDoubleClicked = false;
    mLeftDoubleClicked = false;
    mMiddleDoubleClicked = false;
    mLeftImmediateClicked = false;
    mRightImmediateClicked = false;
    mMiddleImmediateClicked = false;

    glm::dvec2 newCursorPos = glm::dvec2(0);
    glfwGetCursorPos(glfwWindow, &newCursorPos.x, &newCursorPos.y);
    mCursorOffset = glm::vec2(newCursorPos) - mCursorPosition;
    mCursorPosition = newCursorPos;

    CursorMode newCursorMode = GetNativeCursorMode(glfwGetInputMode(glfwWindow, GLFW_CURSOR));
    if (newCursorMode != mCursorMode)
    {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GetGlfwCursorMode(mCursorMode));
        mCursorMode = newCursorMode;
    }
    
    if (mSetCursorPosition)
    {
        glfwSetCursorPos(glfwWindow, mNewCursorPosition.x, mNewCursorPosition.y);
        mCursorPosition = mNewCursorPosition;
        mSetCursorPosition = false;
    }

    ButtonState newLeftState = GetNativeButtonState(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT));
    ButtonState newRightState = GetNativeButtonState(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT));
    ButtonState newMiddleState = GetNativeButtonState(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE));

    if (newLeftState != mLeftState)
    {
        if (newLeftState == ButtonState::Released)
        {
            mLeftImmediateClicked = true;
            mLeftClickCounter++;
        }
        mLeftState = newLeftState;
        mLeftPressedTimer.Start();
    }
    if (newRightState != mRightState)
    {
        if (newRightState == ButtonState::Released)
        {
            mRightImmediateClicked = true;
            mRightClickCounter++;
        }
        mRightState = newRightState;
        mRightPressedTimer.Start();
    }
    if (newMiddleState != mMiddleState)
    {
        if (newMiddleState == ButtonState::Released)
        {
            mMiddleImmediateClicked = true;
            mMiddleClickCounter++;
        }
        mMiddleState = newMiddleState;
        mMiddlePressedTimer.Start();
    }

    if (mLeftPressedTimer.GetElapsedTime() > mDelay)
    {
        if (mLeftClickCounter == 1)
        {
            mLeftClicked = true;
        }
        else if (mLeftClickCounter == 2)
        {
            mLeftDoubleClicked = true;
        }

        mLeftClickCounter = 0;
    }
    if (mRightPressedTimer.GetElapsedTime() > mDelay)
    {
        if (mRightClickCounter == 1)
        {
            mRightClicked = true;
        }
        else if (mRightClickCounter == 2)
        {
            mRightDoubleClicked = true;
        }
        mRightClickCounter = 0;
    }
    if (mMiddlePressedTimer.GetElapsedTime() > mDelay)
    {
        if (mMiddleClickCounter == 1)
        {
            mMiddleClicked = true;
        }
        else if (mMiddleClickCounter == 2)
        {
            mMiddleDoubleClicked = true;
        }
        mMiddleClickCounter = 0;
    }
}