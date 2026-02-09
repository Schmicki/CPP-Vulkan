#pragma once

#include "SGECodeGen.h"
#include "Reflection.h"

u8 skipToChar(const char c, String& code, u32& pos);

u8 skipSpaces(String& code, u32& pos);

u8 skipAllSpaces(String& code, u32& pos);

// skips const strings, comments, preprocessor directives, class scope
u8 skipStuff(String& code, u32& pos, u8 skipPreprocessor);
void analyzeComponent(Class& clazz, String& code, u32& pos);