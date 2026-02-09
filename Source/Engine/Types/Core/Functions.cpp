#include "Functions.h"


namespace sge {

	void memcopy(void* dst, const void* src, u32 elemCount, u32 elemSize, u32 elemOffsetDst, u32 elemOffsetSrc) {
		for (u32 x = 0; x < elemCount; x++) {
			char* _dst = (char*)dst + (szt)x * (szt)elemSize + (szt)x * (szt)elemOffsetDst;
			const char* _src = (const char*)src + (szt)x * (szt)elemSize + (szt)x * (szt)elemOffsetSrc;
			memcpy(_dst, _src, elemSize);
		}
	}
}