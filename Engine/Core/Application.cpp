#include "Application.hpp"
#include <cassert>

void Application::InitializeApplication()
{

	WindowSpecification windowSpecification;
	windowSpecification.size = Vector2u(800, 600);
	windowSpecification.title = "Hello triangle";

	mWindow.reset(new Window());
	mWindow->Create(windowSpecification);
	mWindow->AddListener(BindMember(Application::WindowEventCallback));

	mRenderer.reset(Renderer::Create(*mWindow));
	mAssetManager.reset(new AssetManager());

}

void Application::TerminateApplication()
{
	mWindow->Destroy();
}

void Application::RunApplication()
{
	InitializeApplication();
	Start();
	mRenderer->CreateAssets(mAssetManager);
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
		mWindow->ProcessEvent();
		Update();
	}
}


Application* Application::mInstance = nullptr;