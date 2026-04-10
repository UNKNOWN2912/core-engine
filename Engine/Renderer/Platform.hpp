#pragma once

#define USING_VULKAN

#ifdef USING_VULKAN
#include "Vulkan/VulkanIncludes.hpp"
using PlatformSemaphore = VulkanSemaphore;
using GraphicsData = VulkanGraphicsData;
#endif
