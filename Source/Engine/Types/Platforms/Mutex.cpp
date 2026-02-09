#include "Mutex.h"

#include <stdlib.h>
#include <stdio.h>

#if defined(D_SYSTEM_WINDOWS)
#include <Windows.h>

Mutex::Mutex()
	:handle(CreateMutexW(NULL, false, NULL))
{
}

void Mutex::lock()
{
	WaitForSingleObject(handle, INFINITE);
}

void Mutex::unlock()
{
	ReleaseMutex(handle);
}

void Mutex::destroy()
{
	CloseHandle(handle);
}

#elif defined(D_SYSTEM_LINUX)

#include <pthread.h>

Mutex::Mutex()
{
	handle = new pthread_mutex_t();
	pthread_mutex_init((pthread_mutex_t*)handle, NULL);
}

void Mutex::lock()
{
	pthread_mutex_lock((pthread_mutex_t*)handle);
}

void Mutex::unlock()
{
	pthread_mutex_unlock((pthread_mutex_t*)handle);
}

void Mutex::destroy()
{
	pthread_mutex_destroy((pthread_mutex_t*)handle);
	delete (pthread_mutex_t*)handle;
	handle = NULL;
}

#endif