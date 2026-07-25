#include "Core/Timer.hpp"
#include <Core/Application.hpp>
#include <Core/EntryPoint.hpp>
#if LOG_ALLOCATION
#include <print>
#include <utility>

float GlobalMemoryUsage = 0;
uint32_t GlobalAllocationCount = 0;
uint32_t GlobalDeallocationCount = 0;

void *operator new(size_t size)
{
    std::println("Allocation: {}", size);
    return malloc(size);
}

void *operator new[](size_t size)
{
    std::println("Allocation Array: {}", size);
    return malloc(size);
}

void operator delete(void *ptr, size_t size) noexcept
{
    std::println("DeAllocation: {}", size);
    free(ptr);
}

void operator delete[](void *ptr, size_t size) noexcept
{
    std::println("DeAllocation Array: {}", size);
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
