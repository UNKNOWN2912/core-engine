#include <Core/EntryPoint.hpp>
#include <Core/Application.hpp>
#include "Core/Timer.hpp"

using namespace std; 

float GlobalMemoryUsage = 0;
uint32_t GlobalAllocationCount = 0;
uint32_t GlobalDeallocationCount = 0;

#if LOG_ALLOCATION

void* operator new(size_t size)
{
	return malloc(size);
}

void* operator new[](size_t size)
{
	return malloc(size);
}

void operator delete(void* ptr) noexcept
{
	free(ptr);
}

void operator delete[](void* ptr) noexcept
{
	free(ptr);
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
