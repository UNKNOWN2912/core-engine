#pragma once

struct QueueIndices
{
	uint32_t graphic = UINT32_MAX, present = UINT32_MAX, transfer = UINT32_MAX, compute = UINT32_MAX;
};

struct Queues
{
	VkQueue graphic = VK_NULL_HANDLE, present = VK_NULL_HANDLE, transfer = VK_NULL_HANDLE, compute = VK_NULL_HANDLE;
};
