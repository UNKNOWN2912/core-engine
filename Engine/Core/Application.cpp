#include "Application.hpp"
#include "Input/Mouse.hpp"
#include <cassert>

void Application::InitializeApplication()
{
	CHROME_TRACE_FUNCTION();

	mApplicationTimer.Start();

	WindowSpecification windowSpecification;
	windowSpecification.size = glm::uvec2(800, 600);
	windowSpecification.title = "world";

	mWindow.Create(windowSpecification);
	mWindow.AddListener(BindMember(Application::WindowEventCallback));

	mRenderer.Initialize(mWindow);

}

void Application::TerminateApplication()
{
	CHROME_TRACE_FUNCTION();
	mRenderer.Terminate();
	mWindow.Destroy();
}

void Application::RunApplication()
{
	CHROME_TRACE_FUNCTION();
	InitializeApplication();
	OnStart();
	MainLoop();
	OnEnd();
	TerminateApplication();
}

void Application::Close()
{
	CHROME_TRACE_FUNCTION();
	mRunning = false;
}


bool Application::IsRunning()
{
	CHROME_TRACE_FUNCTION();
	return mRunning;
}

bool Application::WindowEventCallback(uint32_t code, void* data)
{
	CHROME_TRACE_FUNCTION();
	WindowEvent event = (WindowEvent)code;

	switch (event)
	{
		case WindowEvent::WindowClose:
			{
				OnWindowClose();
				break;
			}
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
		case WindowEvent::WindowMousePress:
			{
				MouseButton button = *(MouseButton*)data;
				OnMouseButtonPress(button);
				break;
			}
		case WindowEvent::WindowMouseRelease:
			{
				MouseButton button = *(MouseButton*)data;
				OnMouseButtonRelease(button);
				break;
			}
		case WindowEvent::WindowMinimize:
			OnWindowMinimize();
			break;
		case WindowEvent::WindowMaxmimize:
			OnWindowMaximize();
			break;
		case WindowEvent::WindowMouseMove:
			{
				glm::vec2 position = *(glm::vec2*)data;
				glm::vec2 offset = position - previousMousePos;
				previousMousePos = position; 
				OnMouseMove(position, offset);
				break;
			}
		case WindowEvent::WindowScroll:
			{
				glm::vec2 scroll = *(glm::vec2*)data;
				OnScroll(scroll);
				break;
			}
		case WindowEvent::WindowKeyPress:
			{
				Key key = *(Key*)data;
				OnKeyPress(key);
				break;
			}
		case WindowEvent::WindowKeyRepeat:
			{
				Key key = *(Key*)data;
				OnKeyRepeat(key);
				break;
			}
		case WindowEvent::WindowKeyRelease:
			{
				Key key = *(Key*)data;
				OnKeyRelease(key);
				break;
			}
		case WindowEvent::WindowCharacterType:
			{
				char ch = *(char*)data;
				OnCharacterType(ch);
				break;
			}
	}
		
	return false;
}


Application::Application()
{
	CHROME_TRACE_FUNCTION();
	assert(instance == nullptr);

	instance = this;
}

Application::~Application()
{
	CHROME_TRACE_FUNCTION();
}

float Application::GetDeltaTime() 
{
	return mDeltaTime;
}

float Application::GetElapsedTime() 
{
	return mApplicationTimer.GetElapsedTime();
}

void Application::MainLoop()
{
	CHROME_TRACE_FUNCTION();

	
	while (mRunning)
	{
		mDeltaTimer.Start();
		mWindow.ProcessEvent();
		OnUpdate();
		mDeltaTimer.Stop();
		mDeltaTime = mDeltaTimer.GetDuration();
	}

}

void Application::HideCursor()
{
	CHROME_TRACE_FUNCTION();
	mWindow.HideCursor();
}

void Application::ToggleCursor()
{
	CHROME_TRACE_FUNCTION();

	if(mWindow.isCursorHidden())
		mWindow.ShowCursor();
	else
	 	mWindow.HideCursor();
}

bool Application::IsCursorHidden()
{
	CHROME_TRACE_FUNCTION();
	return mWindow.isCursorHidden();
}

Application* Application::instance = nullptr;