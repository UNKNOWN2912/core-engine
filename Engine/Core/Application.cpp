#include "Application.hpp"
#include <cassert>
#include <print>

void Application::InitializeApplication()
{
	WindowSpecification windowSpecification;
	windowSpecification.size = glm::uvec2(800, 600);
	windowSpecification.title = "world";

	mWindow.Create(windowSpecification);
	mWindow.AddListener(BindMember(Application::WindowEventCallback));

	mRenderer.Initialize(mWindow);

}

void Application::TerminateApplication()
{
	mRenderer.Terminate();
	mWindow.Destroy();
}

void Application::RunApplication()
{
	InitializeApplication();
	Start();
	MainLoop();
	End();
	TerminateApplication();
}

void Application::Close()
{
	mRunning = false;
}


bool Application::IsRunning()
{
	return mRunning;
}


bool Application::WindowEventCallback(uint32_t code, void* data)
{
	WindowEvent event = (WindowEvent)code;

	switch (event)
	{
		case WindowEvent::WindowClose:
			Close();
			break;
		case WindowEvent::WindowResize:
			{
				glm::uvec2 size = *(glm::uvec2*)data;
				OnWindowResize(size);
				break;
			}
		case WindowEvent::WindowMove:
			{
				glm::uvec2 position = *(glm::uvec2*)data;
				OnWindowMove(position);
				break;
			}
		case WindowEvent::WindowMinimize:
			break;
		case WindowEvent::WindowMaxmimize:
			break;

		}
        return false;
}


Application::Application()
{
	assert(instance == nullptr);
}

Application::~Application()
{
}

void Application::MainLoop()
{
	while (mRunning)
	{
		mWindow.ProcessEvent();
		Update();
	}
}


Application* Application::instance = nullptr;