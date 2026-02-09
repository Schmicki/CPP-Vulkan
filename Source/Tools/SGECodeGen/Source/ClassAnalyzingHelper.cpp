#include "ClassAnalyzingHelper.h"

u8 skipToChar(const char c, String& code, u32& pos)
{
	for (; pos < code.size && code[pos] != c; pos++)
	{

	}

	return pos == code.size;
}

u8 skipSpaces(String& code, u32& pos)
{
	for (; pos < code.size && code[pos] == ' '; pos++)
	{

	}

	return pos == code.size;
}

u8 skipAllSpaces(String& code, u32& pos)
{
	for (; pos < code.size && rgxs(code[pos]); pos++)
	{

	}

	return pos == code.size;
}

u8 skipStuff(String& code, u32& pos, u8 skipPreprocessor)
{
	char t = code[pos];
	if (rgxs(t) || (t != '/' && t != '"' && t != 'p' && t != '#'))
		return false;

	String sub;
	code.sub(pos, pos + 12, sub);

	if (sub.beginsWith("//"))
	{
		pos += 2;
		while (pos < code.size && code[pos] != '\n' && code[pos] != '\r')
		{
			pos++;
		}
		pos++;
	}
	else if (sub.beginsWith("/*"))
	{
		pos += 3;
		while (pos < code.size && !(code[pos - 1] == '*' && code[pos] == '/'))
		{
			pos++;
		}
		pos++;
	}
	else if (sub.beginsWith("\""))
	{
		while (pos < code.size && code[pos] != '"')
		{
			pos++;
		}
		pos++;
	}
	else if (skipPreprocessor && sub.beginsWith("#"))
	{
		pos++;;
		while (pos < code.size && !(code[pos - 1] != '\\' && (code[pos] == '\n' || code[pos] == '\r')))
		{
			pos++;
		}
		pos++;
	}
	else if (sub.beginsWith("public:"))
	{
		pos += 7;
	}
	else if (sub.beginsWith("protected:"))
	{
		pos += 10;
	}
	else if (sub.beginsWith("private:"))
	{
		pos += 8;
	}

	return pos >= code.size;
}

void analyzeComponent(Class& clazz, String& code, u32& pos)
{
	for (; pos < code.size && code[pos] != '}'; pos++)
	{
		if (skipAllSpaces(code, pos))
			continue;

		if (skipStuff(code, pos, true))
			continue;

		if (rgxs(code[pos]))
			continue;

		u32 start = pos;
		for (; pos < code.size && code[pos] != '=' && code[pos] != ',' && code[pos] != ';' && code[pos] != '('; pos++)
		{

		}

		switch (code[pos])
		{
		case '=':

			break;
		case ',':

			break;
		case ';':

			break;
		case '(':

			break;
		}
	}
}