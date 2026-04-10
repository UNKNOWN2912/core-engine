#pragma once
#include "Platform.hpp"

class Semaphore : public PlatformSemaphore
{
public:
	void Create() {}
	void Destroy() {}
	void HostWait() {}
};
