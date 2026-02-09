#pragma once

#include "../Aliases.h"

class Thread
{
public:
	Thread(void(*function)(void*), void* args);

	void join();
	void detatch();

	void* handle;
};