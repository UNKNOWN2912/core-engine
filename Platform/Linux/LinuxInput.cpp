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
        mSetCursorPosition = mLockMouse;
    }

    ButtonState newLeftState = GetNativeButtonState(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT));
    ButtonState newRightState = GetNativeButtonState(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT));
    ButtonState newMiddleState = GetNativeButtonState(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE));

    if (newLeftState != mLeftState)
    {
        if (newLeftState == ButtonState::Released)
        {
            if (mIsLeftDragging == false)
                mLeftImmediateClicked = true;

            mIsLeftDragging = false;
            mLeftClickCounter++;
        }
        mLeftState = newLeftState;
        mLeftPressedTimer.Start();
    }
    if (newRightState != mRightState)
    {
        if (newRightState == ButtonState::Released)
        {
            if (mIsRightDragging == false)
                mRightImmediateClicked = true;
            mIsRightDragging = false;
            mRightClickCounter++;
        }
        mRightState = newRightState;
        mRightPressedTimer.Start();
    }
    if (newMiddleState != mMiddleState)
    {
        if (newMiddleState == ButtonState::Released)
        {
            if (mIsMiddleDragging == false)
                mMiddleImmediateClicked = true;
            mIsMiddleDragging = false;
            mMiddleClickCounter++;
        }
        mMiddleState = newMiddleState;
        mMiddlePressedTimer.Start();
    }

    if (mLeftState == ButtonState::Pressed && mCursorOffset != glm::vec2(0) && mIsLeftDragging == false)
    {
        mIsLeftDragging = true;
    }

    if (mRightState == ButtonState::Pressed && mCursorOffset != glm::vec2(0) && mIsLeftDragging == false)
    {
        mIsRightDragging = true;
    }

    if (mMiddleState == ButtonState::Pressed && mCursorOffset != glm::vec2(0) && mIsLeftDragging == false)
    {
        mIsMiddleDragging = true;
    }

    if (mLeftPressedTimer.GetElapsedTime() > mDelay && !mIsLeftDragging)
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
    if (mRightPressedTimer.GetElapsedTime() > mDelay && !mIsRightDragging)
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
    if (mMiddlePressedTimer.GetElapsedTime() > mDelay && !mIsMiddleDragging)
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

Key GetNativeKey(int key)
{
    Key result = Key::None;

    switch (key)
    {

    case GLFW_KEY_SPACE:
        result = Key::Space;
        break;
    case GLFW_KEY_APOSTROPHE:
        result = Key::Apostrophe;
        break;
    case GLFW_KEY_COMMA:
        result = Key::Comma;
        break;
    case GLFW_KEY_MINUS:
        result = Key::Minus;
        break;
    case GLFW_KEY_PERIOD:
        result = Key::Period;
        break;
    case GLFW_KEY_SLASH:
        result = Key::Slash;
        break;
    case GLFW_KEY_0:
        result = Key::Zero;
        break;
    case GLFW_KEY_1:
        result = Key::One;
        break;
    case GLFW_KEY_2:
        result = Key::Two;
        break;
    case GLFW_KEY_3:
        result = Key::Three;
        break;
    case GLFW_KEY_4:
        result = Key::Four;
        break;
    case GLFW_KEY_5:
        result = Key::Five;
        break;
    case GLFW_KEY_6:
        result = Key::Six;
        break;
    case GLFW_KEY_7:
        result = Key::Seven;
        break;
    case GLFW_KEY_8:
        result = Key::Eight;
        break;
    case GLFW_KEY_9:
        result = Key::Nine;
        break;
    case GLFW_KEY_SEMICOLON:
        result = Key::Semicolon;
        break;
    case GLFW_KEY_EQUAL:
        result = Key::Equal;
        break;
    case GLFW_KEY_A:
        result = Key::A;
        break;
    case GLFW_KEY_B:
        result = Key::B;
        break;
    case GLFW_KEY_C:
        result = Key::C;
        break;
    case GLFW_KEY_D:
        result = Key::D;
        break;
    case GLFW_KEY_E:
        result = Key::E;
        break;
    case GLFW_KEY_F:
        result = Key::F;
        break;
    case GLFW_KEY_G:
        result = Key::G;
        break;
    case GLFW_KEY_H:
        result = Key::H;
        break;
    case GLFW_KEY_I:
        result = Key::I;
        break;
    case GLFW_KEY_J:
        result = Key::J;
        break;
    case GLFW_KEY_K:
        result = Key::K;
        break;
    case GLFW_KEY_L:
        result = Key::L;
        break;
    case GLFW_KEY_M:
        result = Key::M;
        break;
    case GLFW_KEY_N:
        result = Key::N;
        break;
    case GLFW_KEY_O:
        result = Key::O;
        break;
    case GLFW_KEY_P:
        result = Key::P;
        break;
    case GLFW_KEY_Q:
        result = Key::Q;
        break;
    case GLFW_KEY_R:
        result = Key::R;
        break;
    case GLFW_KEY_S:
        result = Key::S;
        break;
    case GLFW_KEY_T:
        result = Key::T;
        break;
    case GLFW_KEY_U:
        result = Key::U;
        break;
    case GLFW_KEY_V:
        result = Key::V;
        break;
    case GLFW_KEY_W:
        result = Key::W;
        break;
    case GLFW_KEY_X:
        result = Key::X;
        break;
    case GLFW_KEY_Y:
        result = Key::Y;
        break;
    case GLFW_KEY_Z:
        result = Key::Z;
        break;
    case GLFW_KEY_LEFT_BRACKET:
        result = Key::LeftBracket;
        break;
    case GLFW_KEY_BACKSLASH:
        result = Key::Backslash;
        break;
    case GLFW_KEY_RIGHT_BRACKET:
        result = Key::RightBracket;
        break;
    case GLFW_KEY_GRAVE_ACCENT:
        result = Key::GraveAccent;
        break;
    case GLFW_KEY_WORLD_1:
        result = Key::World1;
        break;
    case GLFW_KEY_WORLD_2:
        result = Key::World2;
        break;

    /* Function keys */
    case GLFW_KEY_ESCAPE:
        result = Key::Escape;
        break;
    case GLFW_KEY_ENTER:
        result = Key::Enter;
        break;
    case GLFW_KEY_TAB:
        result = Key::Tab;
        break;
    case GLFW_KEY_BACKSPACE:
        result = Key::Backspace;
        break;
    case GLFW_KEY_INSERT:
        result = Key::Insert;
        break;
    case GLFW_KEY_DELETE:
        result = Key::Delete;
        break;
    case GLFW_KEY_RIGHT:
        result = Key::Right;
        break;
    case GLFW_KEY_LEFT:
        result = Key::Left;
        break;
    case GLFW_KEY_DOWN:
        result = Key::Down;
        break;
    case GLFW_KEY_UP:
        result = Key::Up;
        break;
    case GLFW_KEY_PAGE_UP:
        result = Key::PageUp;
        break;
    case GLFW_KEY_PAGE_DOWN:
        result = Key::PageDown;
        break;
    case GLFW_KEY_HOME:
        result = Key::Home;
        break;
    case GLFW_KEY_END:
        result = Key::End;
        break;
    case GLFW_KEY_CAPS_LOCK:
        result = Key::CapsLock;
        break;
    case GLFW_KEY_SCROLL_LOCK:
        result = Key::ScrollLock;
        break;
    case GLFW_KEY_NUM_LOCK:
        result = Key::NumLock;
        break;
    case GLFW_KEY_PRINT_SCREEN:
        result = Key::PrintScreen;
        break;
    case GLFW_KEY_PAUSE:
        result = Key::Pause;
        break;
    case GLFW_KEY_F1:
        result = Key::F1;
        break;
    case GLFW_KEY_F2:
        result = Key::F2;
        break;
    case GLFW_KEY_F3:
        result = Key::F3;
        break;
    case GLFW_KEY_F4:
        result = Key::F4;
        break;
    case GLFW_KEY_F5:
        result = Key::F5;
        break;
    case GLFW_KEY_F6:
        result = Key::F6;
        break;
    case GLFW_KEY_F7:
        result = Key::F7;
        break;
    case GLFW_KEY_F8:
        result = Key::F8;
        break;
    case GLFW_KEY_F9:
        result = Key::F9;
        break;
    case GLFW_KEY_F10:
        result = Key::F10;
        break;
    case GLFW_KEY_F11:
        result = Key::F11;
        break;
    case GLFW_KEY_F12:
        result = Key::F12;
        break;
    case GLFW_KEY_F13:
        result = Key::F13;
        break;
    case GLFW_KEY_F14:
        result = Key::F14;
        break;
    case GLFW_KEY_F15:
        result = Key::F15;
        break;
    case GLFW_KEY_F16:
        result = Key::F16;
        break;
    case GLFW_KEY_F17:
        result = Key::F17;
        break;
    case GLFW_KEY_F18:
        result = Key::F18;
        break;
    case GLFW_KEY_F19:
        result = Key::F19;
        break;
    case GLFW_KEY_F20:
        result = Key::F20;
        break;
    case GLFW_KEY_F21:
        result = Key::F21;
        break;
    case GLFW_KEY_F22:
        result = Key::F22;
        break;
    case GLFW_KEY_F23:
        result = Key::F23;
        break;
    case GLFW_KEY_F24:
        result = Key::F24;
        break;
    case GLFW_KEY_F25:
        result = Key::F25;
        break;
    case GLFW_KEY_KP_0:
        result = Key::KP0;
        break;
    case GLFW_KEY_KP_1:
        result = Key::KP1;
        break;
    case GLFW_KEY_KP_2:
        result = Key::KP2;
        break;
    case GLFW_KEY_KP_3:
        result = Key::KP3;
        break;
    case GLFW_KEY_KP_4:
        result = Key::KP4;
        break;
    case GLFW_KEY_KP_5:
        result = Key::KP5;
        break;
    case GLFW_KEY_KP_6:
        result = Key::KP6;
        break;
    case GLFW_KEY_KP_7:
        result = Key::KP7;
        break;
    case GLFW_KEY_KP_8:
        result = Key::KP8;
        break;
    case GLFW_KEY_KP_9:
        result = Key::KP9;
        break;
    case GLFW_KEY_KP_DECIMAL:
        result = Key::KPDecimal;
        break;
    case GLFW_KEY_KP_DIVIDE:
        result = Key::KPDivide;
        break;
    case GLFW_KEY_KP_MULTIPLY:
        result = Key::KPMultiply;
        break;
    case GLFW_KEY_KP_SUBTRACT:
        result = Key::KPSubtract;
        break;
    case GLFW_KEY_KP_ADD:
        result = Key::KPAdd;
        break;
    case GLFW_KEY_KP_ENTER:
        result = Key::KPEnter;
        break;
    case GLFW_KEY_KP_EQUAL:
        result = Key::KPEqual;
        break;
    case GLFW_KEY_LEFT_SHIFT:
        result = Key::LeftShift;
        break;
    case GLFW_KEY_LEFT_CONTROL:
        result = Key::LeftControl;
        break;
    case GLFW_KEY_LEFT_ALT:
        result = Key::LeftAlt;
        break;
    case GLFW_KEY_LEFT_SUPER:
        result = Key::LeftSuper;
        break;
    case GLFW_KEY_RIGHT_SHIFT:
        result = Key::RightShift;
        break;
    case GLFW_KEY_RIGHT_CONTROL:
        result = Key::RightControl;
        break;
    case GLFW_KEY_RIGHT_ALT:
        result = Key::RightAlt;
        break;
    case GLFW_KEY_RIGHT_SUPER:
        result = Key::RightSuper;
        break;
    case GLFW_KEY_MENU:
        result = Key::Menu;
        break;
    }
    return result;
}

int GetGlfwKey(Key key)
{
    int result = 0;

    switch (key)
    {
    case Key::Space:
        result = GLFW_KEY_SPACE;
        break;
    case Key::Apostrophe:
        result = GLFW_KEY_APOSTROPHE;
        break;
    case Key::Comma:
        result = GLFW_KEY_COMMA;
        break;
    case Key::Minus:
        result = GLFW_KEY_MINUS;
        break;
    case Key::Period:
        result = GLFW_KEY_PERIOD;
        break;
    case Key::Slash:
        result = GLFW_KEY_SLASH;
        break;
    case Key::Zero:
        result = GLFW_KEY_0;
        break;
    case Key::One:
        result = GLFW_KEY_1;
        break;
    case Key::Two:
        result = GLFW_KEY_2;
        break;
    case Key::Three:
        result = GLFW_KEY_3;
        break;
    case Key::Four:
        result = GLFW_KEY_4;
        break;
    case Key::Five:
        result = GLFW_KEY_5;
        break;
    case Key::Six:
        result = GLFW_KEY_6;
        break;
    case Key::Seven:
        result = GLFW_KEY_7;
        break;
    case Key::Eight:
        result = GLFW_KEY_8;
        break;
    case Key::Nine:
        result = GLFW_KEY_9;
        break;
    case Key::Semicolon:
        result = GLFW_KEY_SEMICOLON;
        break;
    case Key::Equal:
        result = GLFW_KEY_EQUAL;
        break;
    case Key::A:
        result = GLFW_KEY_A;
        break;
    case Key::B:
        result = GLFW_KEY_B;
        break;
    case Key::C:
        result = GLFW_KEY_C;
        break;
    case Key::D:
        result = GLFW_KEY_D;
        break;
    case Key::E:
        result = GLFW_KEY_E;
        break;
    case Key::F:
        result = GLFW_KEY_F;
        break;
    case Key::G:
        result = GLFW_KEY_G;
        break;
    case Key::H:
        result = GLFW_KEY_H;
        break;
    case Key::I:
        result = GLFW_KEY_I;
        break;
    case Key::J:
        result = GLFW_KEY_J;
        break;
    case Key::K:
        result = GLFW_KEY_K;
        break;
    case Key::L:
        result = GLFW_KEY_L;
        break;
    case Key::M:
        result = GLFW_KEY_M;
        break;
    case Key::N:
        result = GLFW_KEY_N;
        break;
    case Key::O:
        result = GLFW_KEY_O;
        break;
    case Key::P:
        result = GLFW_KEY_P;
        break;
    case Key::Q:
        result = GLFW_KEY_Q;
        break;
    case Key::R:
        result = GLFW_KEY_R;
        break;
    case Key::S:
        result = GLFW_KEY_S;
        break;
    case Key::T:
        result = GLFW_KEY_T;
        break;
    case Key::U:
        result = GLFW_KEY_U;
        break;
    case Key::V:
        result = GLFW_KEY_V;
        break;
    case Key::W:
        result = GLFW_KEY_W;
        break;
    case Key::X:
        result = GLFW_KEY_X;
        break;
    case Key::Y:
        result = GLFW_KEY_Y;
        break;
    case Key::Z:
        result = GLFW_KEY_Z;
        break;
    case Key::LeftBracket:
        result = GLFW_KEY_LEFT_BRACKET;
        break;
    case Key::Backslash:
        result = GLFW_KEY_BACKSLASH;
        break;
    case Key::RightBracket:
        result = GLFW_KEY_RIGHT_BRACKET;
        break;
    case Key::GraveAccent:
        result = GLFW_KEY_GRAVE_ACCENT;
        break;
    case Key::World1:
        result = GLFW_KEY_WORLD_1;
        break;
    case Key::World2:
        result = GLFW_KEY_WORLD_2;
        break;

    /* Function keys */
    case Key::Escape:
        result = GLFW_KEY_ESCAPE;
        break;
    case Key::Enter:
        result = GLFW_KEY_ENTER;
        break;
    case Key::Tab:
        result = GLFW_KEY_TAB;
        break;
    case Key::Backspace:
        result = GLFW_KEY_BACKSPACE;
        break;
    case Key::Insert:
        result = GLFW_KEY_INSERT;
        break;
    case Key::Delete:
        result = GLFW_KEY_DELETE;
        break;
    case Key::Right:
        result = GLFW_KEY_RIGHT;
        break;
    case Key::Left:
        result = GLFW_KEY_LEFT;
        break;
    case Key::Down:
        result = GLFW_KEY_DOWN;
        break;
    case Key::Up:
        result = GLFW_KEY_UP;
        break;
    case Key::PageUp:
        result = GLFW_KEY_PAGE_UP;
        break;
    case Key::PageDown:
        result = GLFW_KEY_PAGE_DOWN;
        break;
    case Key::Home:
        result = GLFW_KEY_HOME;
        break;
    case Key::End:
        result = GLFW_KEY_END;
        break;
    case Key::CapsLock:
        result = GLFW_KEY_CAPS_LOCK;
        break;
    case Key::ScrollLock:
        result = GLFW_KEY_SCROLL_LOCK;
        break;
    case Key::NumLock:
        result = GLFW_KEY_NUM_LOCK;
        break;
    case Key::PrintScreen:
        result = GLFW_KEY_PRINT_SCREEN;
        break;
    case Key::Pause:
        result = GLFW_KEY_PAUSE;
        break;
    case Key::F1:
        result = GLFW_KEY_F1;
        break;
    case Key::F2:
        result = GLFW_KEY_F2;
        break;
    case Key::F3:
        result = GLFW_KEY_F3;
        break;
    case Key::F4:
        result = GLFW_KEY_F4;
        break;
    case Key::F5:
        result = GLFW_KEY_F5;
        break;
    case Key::F6:
        result = GLFW_KEY_F6;
        break;
    case Key::F7:
        result = GLFW_KEY_F7;
        break;
    case Key::F8:
        result = GLFW_KEY_F8;
        break;
    case Key::F9:
        result = GLFW_KEY_F9;
        break;
    case Key::F10:
        result = GLFW_KEY_F10;
        break;
    case Key::F11:
        result = GLFW_KEY_F11;
        break;
    case Key::F12:
        result = GLFW_KEY_F12;
        break;
    case Key::F13:
        result = GLFW_KEY_F13;
        break;
    case Key::F14:
        result = GLFW_KEY_F14;
        break;
    case Key::F15:
        result = GLFW_KEY_F15;
        break;
    case Key::F16:
        result = GLFW_KEY_F16;
        break;
    case Key::F17:
        result = GLFW_KEY_F17;
        break;
    case Key::F18:
        result = GLFW_KEY_F18;
        break;
    case Key::F19:
        result = GLFW_KEY_F19;
        break;
    case Key::F20:
        result = GLFW_KEY_F20;
        break;
    case Key::F21:
        result = GLFW_KEY_F21;
        break;
    case Key::F22:
        result = GLFW_KEY_F22;
        break;
    case Key::F23:
        result = GLFW_KEY_F23;
        break;
    case Key::F24:
        result = GLFW_KEY_F24;
        break;
    case Key::F25:
        result = GLFW_KEY_F25;
        break;
    case Key::KP0:
        result = GLFW_KEY_KP_0;
        break;
    case Key::KP1:
        result = GLFW_KEY_KP_1;
        break;
    case Key::KP2:
        result = GLFW_KEY_KP_2;
        break;
    case Key::KP3:
        result = GLFW_KEY_KP_3;
        break;
    case Key::KP4:
        result = GLFW_KEY_KP_4;
        break;
    case Key::KP5:
        result = GLFW_KEY_KP_5;
        break;
    case Key::KP6:
        result = GLFW_KEY_KP_6;
        break;
    case Key::KP7:
        result = GLFW_KEY_KP_7;
        break;
    case Key::KP8:
        result = GLFW_KEY_KP_8;
        break;
    case Key::KP9:
        result = GLFW_KEY_KP_9;
        break;
    case Key::KPDecimal:
        result = GLFW_KEY_KP_DECIMAL;
        break;
    case Key::KPDivide:
        result = GLFW_KEY_KP_DIVIDE;
        break;
    case Key::KPMultiply:
        result = GLFW_KEY_KP_MULTIPLY;
        break;
    case Key::KPSubtract:
        result = GLFW_KEY_KP_SUBTRACT;
        break;
    case Key::KPAdd:
        result = GLFW_KEY_KP_ADD;
        break;
    case Key::KPEnter:
        result = GLFW_KEY_KP_ENTER;
        break;
    case Key::KPEqual:
        result = GLFW_KEY_KP_EQUAL;
        break;
    case Key::LeftShift:
        result = GLFW_KEY_LEFT_SHIFT;
        break;
    case Key::LeftControl:
        result = GLFW_KEY_LEFT_CONTROL;
        break;
    case Key::LeftAlt:
        result = GLFW_KEY_LEFT_ALT;
        break;
    case Key::LeftSuper:
        result = GLFW_KEY_LEFT_SUPER;
        break;
    case Key::RightShift:
        result = GLFW_KEY_RIGHT_SHIFT;
        break;
    case Key::RightControl:
        result = GLFW_KEY_RIGHT_CONTROL;
        break;
    case Key::RightAlt:
        result = GLFW_KEY_RIGHT_ALT;
        break;
    case Key::RightSuper:
        result = GLFW_KEY_RIGHT_SUPER;
        break;
    case Key::Menu:
        result = GLFW_KEY_MENU;
        break;
    case Key::None:
        result = 0;
        break;
    }
    return result;
}

void Keyboard::ProcessKeyboardEvent(const Window &window)
{
    GLFWwindow *glfwWindow = window.GetNativeWindow();
    for (uint32_t i = 0; i < (uint32_t)Key::MaxEnum; i++)
    {
        int state = glfwGetKey(glfwWindow, GetGlfwKey((Key)i));
        KeyInfo &info = mKeyInfoMap[i];

        ButtonState newState = GetNativeButtonState(state);

        if (newState != info.state)
        {
            if (newState == ButtonState::Released)
            {
                info.clickCounter++;
            }

            info.clickTimer.Start();
            info.state = newState;
        }

        if (info.clickTimer.GetElapsedTime() > mDelay)
        {
            info.clickCounter = 0;
        }
    }
}