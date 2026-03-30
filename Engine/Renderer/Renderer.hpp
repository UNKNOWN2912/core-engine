#pragma once
#include "Core/Window.hpp"
#include "Renderer/Graphic.hpp"

class Renderer
{
public:
	void Initialize(const Window& window);
	Graphic& GetGraphicRef();
private:
	Graphic mGraphic;
};