#pragma once
#include <Core/Window.hpp>
#include "Platform.hpp"

enum class DeviceType
{
	Integrated,
	Dedicated,
	Cpu
};

class Graphics
{
	public:
		static void Initialize(const Window& window, DeviceType deviceType);
		static void Terminate();
		static GraphicsData GetData();
		static GraphicsData& GetDataRef();

		static GraphicsData sData;
};