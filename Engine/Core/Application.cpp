#include "Application.hpp"
#include "Input/Mouse.hpp"
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
			break;
		case WindowEvent::WindowMaxmimize:
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
	assert(instance == nullptr);

	instance = this;
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

void Application::HideCursor()
{
	mWindow.HideCursor();
}

void Application::ToggleCursor()
{
	if(mWindow.isCursorHidden())
		mWindow.ShowCursor();
	else
	 	mWindow.HideCursor();
}

bool Application::IsCursorHidden()
{
	return mWindow.isCursorHidden();
}

Application* Application::instance = nullptr;