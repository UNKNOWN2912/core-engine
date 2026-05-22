#include "Core/Timer.hpp"
#include <Core/Application.hpp>
#include <Core/EntryPoint.hpp>
#include <print>
#include <utility>

float GlobalMemoryUsage = 0;
uint32_t GlobalAllocationCount = 0;
uint32_t GlobalDeallocationCount = 0;

#if LOG_ALLOCATION

void *operator new(size_t size)
{
    // printf("Allocation: %d\n", size);
    return malloc(size);
}

void *operator new[](size_t size)
{
    // printf("Allocation: %d\n", size);
    return malloc(size);
}

void operator delete(void *ptr, size_t size) noexcept
{
    // printf("DeAllocation: %d\n", size);
    free(ptr);
}

void operator delete[](void *ptr, size_t size) noexcept
{
    // printf("DeAllocation: %d\n", size);
    free(ptr);
}

#endif

int main(int argc, char **argv)
{
    StartGlobalTimer();

    Application *application = Application::Create();
    application->RunApplication();
    delete application;
    StopGlobalTimer();
}
