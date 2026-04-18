#include <Core/EntryPoint.hpp>
#include <Core/Application.hpp>
#include <print>
#include "Core/Timer.hpp"

using namespace std; 

int main(int argc, char** argv)
{
	StartGlobalTimer();

	Application* application = Application::Create();
	application->RunApplication();
	delete application;
	StopGlobalTimer();
}
