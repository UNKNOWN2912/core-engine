#include "Renderer.hpp"

void Renderer::Initialize(const Window& window) 
{
	mGraphic.Initialize(window);
}

Graphic& Renderer::GetGraphicRef() 
{
	return mGraphic;
}

