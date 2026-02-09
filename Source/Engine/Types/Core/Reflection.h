#pragma once

#include "Array.h"
#include "String.h"

// SGECodeGen Macros
#define FIELD(...)
#define FUNC(...)
#define CLASS(...)

namespace sge
{
	class UID
	{
	public:
		u64 val;

		UID();
		UID(u64 uid);
		UID(u32 id, u32 hash);
		UID(const UID&) = default;
		UID& operator = (const UID&) = default;
		UID(UID&&) = default;
		UID& operator = (UID&&) = default;
	};

	class Type
	{
	public:
		UID uid;
		String name;
		u32 size;

		virtual void serialize();
	};

	struct Field
	{
		Type* type;
		String name;
	};

	struct Function
	{
		String name;
		Type* type;
		Array<Field> params;
	};

	struct Class : Type
	{
		Class* parent;
		Array<Field> fields;
		Array<Function> functions;

		virtual void serialize() override;
	};
}
