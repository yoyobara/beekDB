#pragma once

#include <string>
#include <openssl/types.h>

using cmd_t = char;

namespace comms
{

	struct message_t
	{
		cmd_t command;
		std::string content;

		message_t(cmd_t command, std::string content) : command(command), content(content) {}
		message_t() : command(0), content(""){}
	};

	/*
	 * sends a message according to protocol. can override content length if needed, elsewhere it shall be according to the message's content size
	 */
	void send_message(SSL* ssl, const message_t& message);

	/*
	 * recives a message according to protocol
	 */
	message_t recv_message(SSL* ssl);
};

namespace comms_constants
{
	const uint16_t DEFAULT_PORT { 1337 };

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
	const std::string QUERY_RES_SUCCESS {"s"};
	const std::string QUERY_RES_ERROR {"e"};

	const comms::message_t JOIN_SUCCESS_MESSAGE(CMD_JOIN_SUCCESS, "");
	const comms::message_t QUERY_RESULT_SUCCESS_NO_CONTENT(CMD_QUERY_RESULT, QUERY_RES_SUCCESS);

}
