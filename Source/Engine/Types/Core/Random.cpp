#include "Random.h"

#include "../Platforms/Time.h"

namespace sge
{
    u32 rand32u()
    {
    	static u32 seed = 390;

    	u32 val = seed;
    	u32 mul = 959677;
    	u32 inc = 1;
    	u32 mod = UINT32_MAX;

    	seed = (seed * mul) % mod + inc;

    	return val;
    }

    u32 rrand32u()
    {
    	static u32 seed = getTime();

    	u32 val = seed;
    	u32 mul = 959677;
    	u32 inc = 1;
    	u32 mod = UINT32_MAX;

    	seed = (seed * mul) % mod + inc;

    	return val;
    }
}