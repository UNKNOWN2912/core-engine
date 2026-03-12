#pragma once
#include <Core/Window.hpp>

class GraphicContext
{
public:
	static GraphicContext* GetCurrentContext();
	static GraphicContext* Create(const Window& window);

	virtual void SetAsCurrentContext() = 0;
};