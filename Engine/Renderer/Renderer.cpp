#include "Renderer.hpp"
#include <Core/Window.hpp>
#include "Graphics.hpp"
#include "CommandBuffer.hpp"

void Renderer::Initialize(const Window& window)
{
	Graphics::Initialize(window, DeviceType::Dedicated);
}

void Renderer::Terminate()
{
	Graphics::Terminate();
}
