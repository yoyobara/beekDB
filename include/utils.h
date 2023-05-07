#pragma once

#include <openssl/ssl.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>

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

namespace ssl_utils
{
	inline int write_s(SSL* ssl, const std::string& str)
	{
		return SSL_write(ssl, str.data(), str.size());
	}
	
	inline std::string read_s(SSL* ssl, int n)
	{
		std::vector<char> buff(n);
		SSL_read(ssl, buff.data(), buff.size());

		return std::string(buff.begin(), buff.end());
	}
}
