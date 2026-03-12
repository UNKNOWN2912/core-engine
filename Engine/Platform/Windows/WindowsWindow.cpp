#define GLFW_INCLUDE_VULKAN
#include <Core/Window.hpp>
#include <GLFW/glfw3.h>
#include <cassert>
#include <print>
#include <Core/Macro.hpp>
#include <Renderer/Vulkan/VulkanGraphicData.hpp>


struct WindowsWindowPlatformData
{
	GLFWwindow* window = nullptr;
	static bool glfwInitialized;
	EventDispatcher dispatcher;
};

bool WindowsWindowPlatformData::glfwInitialized = false;

void windowCloseCallback(GLFWwindow* window)
{
	LOG("window close callback");
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)glfwGetWindowUserPointer(window);
	platformData->dispatcher.Dispatch((uint32_t)WindowEvent::WindowClose, nullptr);
}

void Window::Create(const WindowSpecification& specification)
{
	LOG("creating window");

	WindowsWindowPlatformData* platformData = new WindowsWindowPlatformData;
	mPlatformData = platformData;

	if (platformData->glfwInitialized == false)
	{
		glfwInit();
		platformData->glfwInitialized = true;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	platformData->window = glfwCreateWindow(specification.size.x, specification.size.y, specification.title.c_str(), nullptr, nullptr);

	if (platformData->window == nullptr)
	{
		ERROR("Failed to create window");
		return;
	}

	glfwSetWindowUserPointer(platformData->window, mPlatformData);
	glfwSetWindowCloseCallback(platformData->window, windowCloseCallback);

}

void Window::Destroy()
{
	LOG("destroying window");

	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	assert(platformData->window != nullptr);
	glfwDestroyWindow(platformData->window);
	platformData->window = nullptr;
}

Vector2u Window::GetSize() const
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	int width, height;
	glfwGetWindowSize(platformData->window, &width, &height);

	return Vector2u(width, height);
}

Vector2u Window::GetPosition() const
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	int x, y;
	glfwGetWindowPos(platformData->window, &x, &y);

	return Vector2u(x, y);
}

std::string Window::GetTitle() const
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	return glfwGetWindowTitle(platformData->window);
}


void Window::SetSize(const Vector2u& size)
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	glfwSetWindowSize(platformData->window, size.x, size.y);
}

void Window::SetPosition(const Vector2u& position)
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	glfwSetWindowPos(platformData->window, position.x, position.y);
}

void Window::SetTitle(const std::string& title)
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	glfwSetWindowTitle(platformData->window, title.c_str());
}

void Window::AddListener(std::function<bool(uint32_t code, void* data)> listener)
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	platformData->dispatcher.AddListener(listener);
}

void Window::ProcessEvent()
{
	glfwPollEvents();
}

WindowGraphicData* Window::GetGraphicData() const
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;

	VulkanWindowGraphicData* graphicData = new VulkanWindowGraphicData();

	uint32_t extensionCount;
	const char** extension = glfwGetRequiredInstanceExtensions(&extensionCount);

	for (int i = 0; i < extensionCount; i++)
	{
		graphicData->instanceExtensions.push_back(extension[i]);
	}

	return graphicData;
}


void* Window::GetNativeWindow() const
{
	WindowsWindowPlatformData* platformData = (WindowsWindowPlatformData*)mPlatformData;
	return platformData->window;
}