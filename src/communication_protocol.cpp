#include "communication_protocol.h"
#include <cstdint>
#include <iostream>
#include "table/table_storage_constants.h"
#include "utils.h"
#include <cstring>
#include <sys/types.h>

comms::message_t::message_t(){}
comms::message_t::message_t(comms::message_t&& msg)
{
	command = msg.command;
	content = std::move(msg.content);
}

void comms::send_message(const Socket &s, const message_t& message) // length is -1 by default
{
	s.send(message.command + encode(message.content.size()) + message.content);
}

comms::message_t comms::recv_message(const Socket &s)
{
	comms::message_t msg{};

	msg.command = s.recv(comms_constants::CMD_LENGTH)[0];

	// get length
	std::string length_str {s.recv(comms_constants::LENGTH_LENGTH)};
	uint64_t length;
	std::memcpy(&length, length_str.data(), comms_constants::LENGTH_LENGTH);


	// get content
	for (int i = 0 ; i < length / comms_constants::BATCH_SIZE ; i++)
		msg.content += s.recv(comms_constants::BATCH_SIZE);
	msg.content += s.recv(length % comms_constants::BATCH_SIZE);

	return msg;
}
