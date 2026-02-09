#pragma once

#include "Types/Aliases.h"

namespace sge
{
    /*
    * Return a number with 9 to 10 digits
    * 0102030405
    * 01 - Month
    * 02 - Day
    * 03 - Hour
    * 04 - Minute
    * 05 - Second
    */
    u32 getTime();
    
    /*
    * Some unsynchronized time in nanoseconds
    * Can be used for game tick delta, etc.
    */
    u64 getClockTime();
}