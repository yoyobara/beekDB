#pragma once

#include "cpp_socket.h"
#include <string>
#include <utility>

namespace comms_constants
{
	const int CMD_LENGTH {1};
	const int LENGTH_LENGTH { sizeof (unsigned long) };
	const int BATCH_SIZE {2048};
}

namespace comms
{
	using cmd_t = char;

	struct message_t
	{
		cmd_t command;
		std::string content;

		message_t(message_t&& msg);
		message_t();
	};

	/*
	 * sends a message according to protocol
	 */
	void send_message(const Socket& s, const message_t& message);

	/*
	 * recives a message according to protocol
	 */
	message_t recv_message(const Socket& s);
	
};
