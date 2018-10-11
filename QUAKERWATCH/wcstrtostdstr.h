#pragma once

#include <string>

template<int buf_size = 16>
std::string wcstrtostdstr(const wchar_t* in)
{
	char out[buf_size];
	wcstombs(out, in, buf_size);
	return out;
}