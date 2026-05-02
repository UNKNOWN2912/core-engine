#include "Input/Keyboard.hpp"
#include "Input/Mouse.hpp"
#define GLFW_INCLUDE_VULKAN
#include <Core/Window.hpp>
#include <GLFW/glfw3.h>
#include <cassert>
#include <Core/Macro.hpp>


struct WindowData
{
	GLFWwindow* window = nullptr;
	EventDispatcher dispatcher;
};

void windowCloseCallback(GLFWwindow* window)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowClose, nullptr);
}

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	glm::uvec2 size = { width, height };
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowResize, &size);
}

void windowMoveCallback(GLFWwindow* window, int x, int y)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	glm::vec2 position = { x, y };
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowMove, &position);
}

void mouseMoveCallback(GLFWwindow* window, double x, double y)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	glm::vec2 position = { x, y };
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowMouseMove, &position);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	MouseButton mouseButton;

	if(button == GLFW_MOUSE_BUTTON_LEFT)
		mouseButton = MouseButton::Left;
	else if(button == GLFW_MOUSE_BUTTON_RIGHT)
		mouseButton = MouseButton::Right;
	else if(button == GLFW_MOUSE_BUTTON_MIDDLE)
		mouseButton = MouseButton::Middle;

	if(action == GLFW_PRESS)
	{
		platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowMousePress, &mouseButton);
	}
	else if(action == GLFW_RELEASE)
	{
		platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowMouseRelease, &mouseButton);
	}
}

void mouseScrollCallback(GLFWwindow* window, double x, double y)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	glm::vec2 scroll = { x, y };
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowScroll, &scroll);

}

Key GetKeyFromGlfwKey(int key)
{
	Key result;

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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	
	Key result = GetKeyFromGlfwKey(key);

	if(action == GLFW_PRESS)
	{
		platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowKeyPress, &result);
	}
	else if(action == GLFW_REPEAT)
	{
		platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowKeyRepeat, &result);
	}
	else if(action == GLFW_RELEASE)
	{
		platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowKeyRelease, &result);
	}
}

void characterCallback(GLFWwindow* window, unsigned int codepoint)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	char ch = codepoint;
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowCharacterType, &ch);
}

void minimizeCallback(GLFWwindow* window, int minimize)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	bool isMinimized = minimize;
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowMinimize, &isMinimized);
}

void maximizeCallback(GLFWwindow* window, int maximize)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	bool isMaximized = maximize;
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowMaximize, &isMaximized);
}

void Window::Create(const WindowSpecification& specification)
{
    CHROME_TRACE_FUNCTION();
	mData = new WindowData();

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	mData->window = glfwCreateWindow(specification.size.x, specification.size.y, specification.title.c_str(), nullptr, nullptr);

	if (mData->window == nullptr)
	{
		const char* description;
		glfwGetError(&description);
		ERROR("Failed to create window: {}", description);
		return;
	}

	glfwSetWindowUserPointer(mData->window, mData);
	glfwSetWindowCloseCallback(mData->window, windowCloseCallback);
	glfwSetWindowSizeCallback(mData->window, windowResizeCallback);
	glfwSetWindowPosCallback(mData->window, windowMoveCallback);
	glfwSetCursorPosCallback(mData->window, mouseMoveCallback);
	glfwSetMouseButtonCallback(mData->window, mouseButtonCallback);
	glfwSetScrollCallback(mData->window, mouseScrollCallback);
	glfwSetKeyCallback(mData->window, keyCallback);
	glfwSetCharCallback(mData->window, characterCallback);
	glfwSetWindowIconifyCallback(mData->window, minimizeCallback);
	glfwSetWindowMaximizeCallback(mData->window, maximizeCallback);
}

void Window::HideCursor()
{
    CHROME_TRACE_FUNCTION();
	glfwSetInputMode(mData->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Window::Destroy()
{
    CHROME_TRACE_FUNCTION();
	assert(mData->window != nullptr);
	glfwDestroyWindow(mData->window);

	delete mData;
}

glm::uvec2 Window::GetSize() const
{
    CHROME_TRACE_FUNCTION();
	int width, height;
	glfwGetWindowSize(mData->window, &width, &height);

	return glm::uvec2(width, height);
}

glm::uvec2 Window::GetFrameBufferSize() const
{
    CHROME_TRACE_FUNCTION();
	int width, height;
	glfwGetFramebufferSize(mData->window, &width, &height);

	return glm::uvec2(width, height);
}

glm::uvec2 Window::GetPosition() const
{
    CHROME_TRACE_FUNCTION();
	int x, y;
	glfwGetWindowPos(mData->window, &x, &y);

	return glm::uvec2(x, y);
}

std::string Window::GetTitle() const
{
    CHROME_TRACE_FUNCTION();
	return glfwGetWindowTitle(mData->window);
}


void Window::SetSize(const glm::uvec2& size)
{
    CHROME_TRACE_FUNCTION();
	glfwSetWindowSize(mData->window, size.x, size.y);
}

void Window::SetPosition(const glm::uvec2& position)
{
    CHROME_TRACE_FUNCTION();
	glfwSetWindowPos(mData->window, position.x, position.y);
}

void Window::SetTitle(const std::string& title)
{
    CHROME_TRACE_FUNCTION();
	glfwSetWindowTitle(mData->window, title.c_str());
}

void Window::AddListener(std::function<bool(uint32_t code, void* data)> listener)
{
    CHROME_TRACE_FUNCTION();
	mData->dispatcher.AddListener(listener);
}

void Window::ProcessEvent()
{
    CHROME_TRACE_FUNCTION();
	glfwPollEvents();
}

void* Window::GetNativeWindow() const
{
    CHROME_TRACE_FUNCTION();
	return mData->window;
}


void Window::ShowCursor()
{
    CHROME_TRACE_FUNCTION();
	glfwSetInputMode(mData->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
bool Window::isCursorHidden()
{
    CHROME_TRACE_FUNCTION();
	return glfwGetInputMode(mData->window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL;
}
bool Window::isFullscreen()
{
    CHROME_TRACE_FUNCTION();
	return glfwGetWindowMonitor(mData->window) == glfwGetPrimaryMonitor();
}

void Window::SetFullscreen(bool fullscreen)
{
    CHROME_TRACE_FUNCTION();

	if(fullscreen)
	{
		glfwSetWindowMonitor(mData->window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 0);
	}
	else 
	{
		glfwSetWindowMonitor(mData->window, nullptr, 0, 0, 800, 600, 0);
	}
}