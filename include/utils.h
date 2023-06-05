#pragma once

#include <string>
#include <thread>
#include <sstream>
#include <vector>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "communication_protocol.h"

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

inline void send_query_result(SSL* ssl, bool is_ok, std::string&& data)
{
	comms::message_t msg{comms_constants::CMD_QUERY_RESULT, (is_ok ? comms_constants::QUERY_RES_SUCCESS : comms_constants::QUERY_RES_ERROR) + data };
	comms::send_message(ssl, msg);
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

	inline std::string get_ssl_error()
	{
		unsigned long err = ERR_get_error();
		char buff[256]{};
		ERR_error_string(err, buff);

		return std::string(buff);
	}
}
