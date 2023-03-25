#pragma once

#include <string>
#include <sys/types.h>

template<typename T>
inline std::string encode(T val)
{
	std::string s(sizeof(T), 0);
	*static_cast<T*>(static_cast<void*>(s.data())) = val;

	return s;
}

