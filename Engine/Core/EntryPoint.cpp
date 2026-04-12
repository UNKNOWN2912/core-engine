#include <Core/EntryPoint.hpp>
#include <Core/Application.hpp>
#include <print>

using namespace std; 

int main(int argc, char** argv)
{
	Application* application = Application::Create();
	application->RunApplication();
	delete application;
}
