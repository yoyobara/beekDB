#include "communication_protocol.h"
#include <cstdint>
#include <iostream>
#include "table/table_storage_constants.h"
#include "utils.h"
#include <cstring>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <spdlog/fmt/bin_to_hex.h>

void comms::send_message(const Socket &s, const message_t& message) // length is -1 by default
{
	spdlog::debug("sending message (command: '{}', content: '{:n}', content_size: {} bytes)", message.command, spdlog::to_hex(message.content.begin(), message.content.end()), message.content.size());
	s.send(message.command + encode(message.content.size()) + message.content);
	spdlog::debug("sent");
}

comms::message_t comms::recv_message(const Socket &s)
{
	spdlog::debug("recieving message");
	comms::message_t msg;

	msg.command = s.recv(comms_constants::CMD_LENGTH)[0];

	spdlog::debug("command is '{}'", msg.command);

	// get length
	std::string length_str {s.recv(comms_constants::LENGTH_LENGTH)};
	uint64_t length;
	std::memcpy(&length, length_str.data(), comms_constants::LENGTH_LENGTH);

	spdlog::debug("length is {} bytes", length);

	// get content
	if (length != 0)
	{
		for (int i = 0 ; i < length / comms_constants::BATCH_SIZE ; i++)
			msg.content += s.recv(comms_constants::BATCH_SIZE);
		msg.content += s.recv(length % comms_constants::BATCH_SIZE);
	}

	spdlog::debug("recieved message (command: '{}', content: '{}', content_size: {} bytes)", msg.command, spdlog::to_hex(msg.content.begin(), msg.content.end()), msg.content.size());

	return msg;
}
