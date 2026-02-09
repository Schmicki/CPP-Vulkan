#pragma once

#include "SGECodeGen.h"

struct Field {
	String name;
	String type;
};

struct Function {
	String name;
	String type;
	Array<Field> params;
};

struct Class {
	String name;
	Array<Field> fields;
	Array<Function> functions;
	Array<Class*> parents;
};