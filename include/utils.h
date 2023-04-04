#pragma once

#include <sstream>
#include <string>
#include <sys/types.h>
#include <thread>

template<typename T>
inline std::string encode(T val)
{
	std::string s(sizeof(T), 0);
	*static_cast<T*>(static_cast<void*>(s.data())) = val;

	return s;
}

inline std::string get_thread_id()
{
	return (std::stringstream() << std::this_thread::get_id()).str();
}
