#include "Renderer.hpp"

void Renderer::Initialize(const Window& window)
{
	Graphics::Initialize(window);
}

void Renderer::Terminate()
{
	Graphics::Terminate();
}
