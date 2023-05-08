#pragma once

#include <openssl/ssl.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>
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

		spdlog::debug("shutdown: {}", SSL_get_shutdown(ssl));

		// TODO bug
		SSL_read(ssl, buff.data(), buff.size());
		//

		spdlog::debug("read res {}", spdlog::to_hex(buff.begin(), buff.end()));

		spdlog::debug("recived by ssl: {}", spdlog::to_hex(buff.begin(), buff.end()));
		return std::string(buff.begin(), buff.end());
	}
}
