#include "Reflection.h"

namespace sge
{
	UID::UID()
		:val(0)
	{
	}

	UID::UID(u64 uid)
		:val(uid)
	{
	}

	UID::UID(u32 id, u32 hash)
	{
		as<u32*>(&val)[0] = id;
		as<u32*>(&val)[1] = hash;
	}

	void Type::serialize()
	{
	}

	void Class::serialize()
	{
	}
}
