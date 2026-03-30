#define GLFW_INCLUDE_VULKAN
#include <Core/Window.hpp>
#include <GLFW/glfw3.h>
#include <cassert>
#include <Core/Macro.hpp>


struct WindowData
{
	GLFWwindow* window = nullptr;
	EventDispatcher dispatcher;
	static bool glfwInitialized;
};

bool WindowData::glfwInitialized = false;

void windowCloseCallback(GLFWwindow* window)
{
	WindowData* platformData = (WindowData*)glfwGetWindowUserPointer(window);
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowClose, nullptr);
}

void Window::Create(const WindowSpecification& specification)
{
	mData = new WindowData();

	if (mData->glfwInitialized == false)
	{
		glfwInit();
		mData->glfwInitialized = true;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	mData->window = glfwCreateWindow(specification.size.x, specification.size.y, specification.title.c_str(), nullptr, nullptr);

	if (mData->window == nullptr)
	{
		ERROR("Failed to create window");
		return;
	}

	glfwSetWindowUserPointer(mData->window, mData);
	glfwSetWindowCloseCallback(mData->window, windowCloseCallback);

}

void Window::Destroy()
{
	assert(mData->window != nullptr);
	glfwDestroyWindow(mData->window);

	delete mData;
	mData->window = nullptr;
}

glm::uvec2 Window::GetSize() const
{
	int width, height;
	glfwGetWindowSize(mData->window, &width, &height);

	return glm::uvec2(width, height);
}

glm::uvec2 Window::GetPosition() const
{
	int x, y;
	glfwGetWindowPos(mData->window, &x, &y);

	return glm::uvec2(x, y);
}

std::string Window::GetTitle() const
{
	return glfwGetWindowTitle(mData->window);
}


void Window::SetSize(const glm::uvec2& size)
{
	glfwSetWindowSize(mData->window, size.x, size.y);
}

void Window::SetPosition(const glm::uvec2& position)
{
	glfwSetWindowPos(mData->window, position.x, position.y);
}

void Window::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(mData->window, title.c_str());
}

void Window::AddListener(std::function<bool(uint32_t code, void* data)> listener)
{
	mData->dispatcher.AddListener(listener);
}

void Window::ProcessEvent()
{
	glfwPollEvents();
}

void* Window::GetNativeWindow() const
{
	return mData->window;
}