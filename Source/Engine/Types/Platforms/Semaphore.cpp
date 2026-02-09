#include "Semaphore.h"

#if defined(D_SYSTEM_WINDOWS)

#include <Windows.h>

// Creates unsignaled Semaphore with max signals 1
Semaphore::Semaphore()
	:handle(CreateSemaphoreW(NULL, 0, 1, NULL))
{
	
}

Semaphore::Semaphore(u32 signals, u32 maxSignals)
	:handle(CreateSemaphore(NULL, signals, maxSignals, NULL))
{

}

void Semaphore::signal()
{
	ReleaseSemaphore(handle, 1, NULL);
}

void Semaphore::wait()
{
	WaitForSingleObject(handle, INFINITE);
}

void Semaphore::destroy()
{
	CloseHandle(handle);
}

#elif defined(D_SYSTEM_LINUX)

#include <semaphore.h>

struct SemHandle
{
	sem_t sem;
	u32 maxSignals;
};

// Creates unsignaled Semaphore with max signals 1
Semaphore::Semaphore()
{
	handle = new SemHandle();
	sem_init((sem_t*)handle, 0, 0);
	((SemHandle*)handle)->maxSignals = 1;
}

Semaphore::Semaphore(u32 signals, u32 maxSignals)
{
	handle = new SemHandle();
	sem_init((sem_t*)handle, 0, signals);
	((SemHandle*)handle)->maxSignals = maxSignals;
}

void Semaphore::signal()
{
	int value;
	sem_getvalue((sem_t*)handle, &value);
	if ((u32)value < ((SemHandle*)handle)->maxSignals)
	{
		sem_post((sem_t*)handle);
	}
}

void Semaphore::wait()
{
	sem_wait((sem_t*)handle);
}

void Semaphore::destroy()
{
	sem_destroy((sem_t*)handle);
	delete (SemHandle*)handle;
}

#endif