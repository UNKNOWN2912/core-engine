#include <Core/EntryPoint.hpp>
#include <Core/Application.hpp>

using namespace std; 

int main(int argc, char** argv)
{
	Application* application = Application::Create();
	application->RunApplication();
	delete application;

}
