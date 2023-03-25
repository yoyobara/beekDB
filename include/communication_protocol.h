#pragma once

#include "cpp_socket.h"
#include <string>
#include <utility>

using cmd_t = char;

namespace comms_constants
{
	const int CMD_LENGTH {1};
	const int LENGTH_LENGTH { sizeof(uint64_t) };
	const int BATCH_SIZE {2048};

	// commands from client
	const cmd_t CMD_JOIN {'j'};
	const cmd_t CMD_LEAVE {'l'};
	const cmd_t CMD_QUERY {'q'};

	// commands from server
	const cmd_t CMD_JOIN_SUCCESS {'J'};
	const cmd_t CMD_QUERY_RESULT {'Q'};
	const cmd_t CMD_TERMINATION {'T'};

	// QUERY RESULTS
	const cmd_t QUERY_RES_SUCCESS {'s'};
	const cmd_t QUERY_RES_ERROR {'e'};
}

namespace comms
{

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
