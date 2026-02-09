#include "Time.h"

#if defined(D_SYSTEM_LINUX)

#include <time.h>

namespace sge
{
    u32 getTime()
    {
        time_t tp = time(NULL);
        struct tm* ct = localtime(&tp);

        u64 time = (ct->tm_mon + 1) * 100000000;
        time += ct->tm_mday * 1000000;
        time += ct->tm_hour * 10000;
        time += ct->tm_min * 100;
        time += ct->tm_sec;

        return time;
    }

    u64 getClockTime()
    {
        timespec tp;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
        return tp.tv_sec * 1000000000 + tp.tv_nsec;
    }
}

#elif defined(D_SYSTEM_WINDOWS)

#include <time.h>
#include <Windows.h>

namespace sge
{
    u32 getTime()
    {
        time_t tp = time(NULL);
        struct tm ct;
        localtime_s(&ct, &tp);

        u32 time = 100000000 * (ct.tm_mon + 1);
        time += ct.tm_mday * 1000000;
        time += ct.tm_hour * 10000;
        time += ct.tm_min * 100;
        time += ct.tm_sec;

        return time;
    }

    u64 getClockTime()
    {
        LARGE_INTEGER frequency, counter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        
        u64 nsec = (((counter.QuadPart - (counter.QuadPart / 1000000000) * 1000000000) * 1000000000) / frequency.QuadPart) % 1000000000;
        u64 sec = counter.QuadPart / frequency.QuadPart;

        return sec * 1000000000 + nsec;
    }
}

#endif
