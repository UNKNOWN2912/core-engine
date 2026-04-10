#pragma once
#include "Enum.hpp"
#include <vector>
#include "Semaphore.hpp"

class CommandBuffer : public PlatformCommandBuffer
{
public:
	void Create();

	void Reset();

	void BeginRecording();
	void EndRecording();

	void Execute(QueueType queueType, Semaphore waitSemaphore, Semaphore signalSemaphore, PipelineStage stages);
};

