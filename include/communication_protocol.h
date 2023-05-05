#pragma once

#include "cpp_socket.h"
#include <cryptopp/integer.h>
#include <string>
#include <sys/types.h>
#include <utility>

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
	void send_message(const Socket& s, const message_t& message);

	/*
	 * recives a message according to protocol
	 */
	message_t recv_message(const Socket& s);
	
};

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
	const std::string QUERY_RES_SUCCESS {"s"};
	const std::string QUERY_RES_ERROR {"e"};

	const comms::message_t JOIN_SUCCESS_MESSAGE(CMD_JOIN_SUCCESS, "");
	const comms::message_t QUERY_RESULT_SUCCESS_NO_CONTENT(CMD_QUERY_RESULT, QUERY_RES_SUCCESS);

	// diffie - hellman parameters
	const CryptoPP::Integer DH_MODULUS("0x82f043b2edae3f093b78aedf79c9f36235ccd721eebb4ff36caa1fc27f952457985e486d7f68177014ff605b57a635c943867556fd409672e416ef1ac4ae0f0573f48130901c580b5637eac68d4cbde4246bc5aa20b53ae7dc4c1569886d7efa3b144672efd1d16b06c93f1d343c93402e9f1001408a722dd7be00e0ab6d757b");

	const CryptoPP::Integer DH_GENERATOR(0x2);
}
