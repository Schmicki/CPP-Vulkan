#pragma once

#include "../Aliases.h"

class Mutex
{
public:
	Mutex();
	void lock();
	void unlock();
	void destroy();

	void* handle;
};