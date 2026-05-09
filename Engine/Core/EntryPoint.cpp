#include <Core/EntryPoint.hpp>
#include <Core/Application.hpp>
#include "Core/Timer.hpp"

using namespace std; 


#if LOG_ALLOCATION

void* operator new(size_t size)
{
	static uint32_t count = 0;
	printf("allocation: %li count: %u\n", size, count++);
	return malloc(size);
}

void* operator new[](size_t size)
{
	static uint32_t count = 0;
	printf("allocation array: %li count: %u\n", size, count++);
	return malloc(size);
}

#endif

int main(int argc, char** argv)
{
	StartGlobalTimer();

	Application* application = Application::Create();
	application->RunApplication();
	delete application;
	StopGlobalTimer();
}
