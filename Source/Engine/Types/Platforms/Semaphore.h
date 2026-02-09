#pragma once

#include "../Aliases.h"

class Semaphore
{
public:
	Semaphore();
	Semaphore(u32 signals, u32 maxSignals);

	void signal();
	void wait();
	void destroy();

	void* handle;
};