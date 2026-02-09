#pragma once
#include "../Aliases.h"
#include "TypeInfo.h"


namespace sge {
	void memcopy(void* dst, const void* src, u32 elemCount, u32 elemSize, u32 elemOffsetDst, u32 elemOffsetSrc);
}