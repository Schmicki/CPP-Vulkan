#include "Thread.h"

#if defined(D_SYSTEM_WINDOWS)

#include <Windows.h>

struct _l_ {
	void(*function)(void*);
	void* args;
};

DWORD wrapper(void* args) {
	_l_* l = reinterpret_cast<_l_*>(args);
	l->function(l->args);
	delete l;
	return 0;
}

Thread::Thread(void(*function)(void*), void* args)
{
	_l_* _args = new _l_{ function, args };
	handle = CreateThread(NULL, 0, wrapper, _args, 0, NULL);
}

void Thread::join()
{
	WaitForSingleObject(handle, INFINITE);
	CloseHandle(handle);
}

void Thread::detatch()
{
	CloseHandle(handle);
}

#elif defined(D_SYSTEM_LINUX)

#include <pthread.h>

struct _l_ {
	void(*function)(void*);
	void* args;
};

void* wrapper(void* args) {
	_l_* l = reinterpret_cast<_l_*>(args);
	l->function(l->args);
	delete l;
	return NULL;
}

Thread::Thread(void(*function)(void*), void* args)
{
	_l_* _args = new _l_{function, args};
	handle = new pthread_t();
	if(pthread_create((pthread_t*)handle, NULL, wrapper, _args) != 0)
	{
		delete (pthread_t*)handle;
		handle == NULL;
	}
}

void Thread::join()
{
	pthread_join(*(pthread_t*)handle, NULL);
	delete (pthread_t*)handle;
}

void Thread::detatch()
{
	pthread_detach(*(pthread_t*)handle);
	delete (pthread_t*)handle;
}

#endif