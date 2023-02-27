#include "utils.h"

template<typename T>
std::string encode(T&& val)
{
	std::string s(sizeof(T), 0);
	*static_cast<T*>(s.data()) = val;

	return s;
}
