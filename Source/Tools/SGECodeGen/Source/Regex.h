#pragma once

inline bool rgxw(const char c)
{
	return c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_';
}

inline bool rgxW(const char c)
{
	return !rgxw(c);
}

inline bool rgxs(const char c) {
	return c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\r';
}

inline bool rgxS(const char c) {
	return !rgxs(c);
}

inline bool rgxd(const char c) {
	return c >= '0' && c <= '9';
}

inline bool rgxD(const char c) {
	return !rgxd(c);
}

inline bool rgxn(const char c) {
	return c == '\n';
}

inline bool rgxN(const char c) {
	return !rgxn(c);
}

inline bool rgxdot(const char c) {
	return c != '\n' && c != '\r';
}

inline bool rgxDot(const char c) {
	return !rgxdot(c);
}