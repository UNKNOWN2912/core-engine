#include "Application.hpp"
#include <cassert>
#include <print>

void Application::InitializeApplication()
{
	WindowSpecification windowSpecification;
	windowSpecification.size = glm::uvec2(800, 600);
	windowSpecification.title = "Hello triangle";

	mWindow.Create(windowSpecification);
	mWindow.AddListener(BindMember(Application::WindowEventCallback));

	mRenderer.Initialize(mWindow);
}

void Application::TerminateApplication()
{
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
	if (code == (uint32_t)WindowEvent::WindowClose)
	{
		Close();
	}

	std::println("Data: {}", data);

	return false;
}


Application::Application()
{
	assert(mInstance == nullptr);
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


Application* Application::mInstance = nullptr;