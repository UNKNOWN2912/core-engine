#pragma once
#include <Core/Window.hpp>

enum class DeviceType
{
	Integrated,
	Dedicated,
	Cpu
};

struct GraphicsData;

class Graphics
{
	public:
		static void Initialize(const Window& window, DeviceType deviceType);
		static void Terminate();
		static GraphicsData GetGraphicsData();

		static GraphicsData sData;
};