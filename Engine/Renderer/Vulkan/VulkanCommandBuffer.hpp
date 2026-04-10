#pragma once

class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer()

	void Create();

	void Reset();

	void BeginRecording();
	void EndRecording();

	void Execute(QueueType queueType, Semaphore waitSemaphore, Semaphore signalSemaphore, PipelineStage stages);
private:
	GraphicsData& mData;
};

