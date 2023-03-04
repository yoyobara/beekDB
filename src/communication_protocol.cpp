#include "communication_protocol.h"
#include "utils.h"
#include <cstring>

void comms::send_message(const Socket &s, const message_t& message)
{
	s.send(message.command + encode(message.content.size()) + message.content);
}

comms::message_t comms::recv_message(const Socket &s)
{
	comms::message_t msg;
	msg.command = s.recv(comms_constants::CMD_LENGTH)[0];
	
	// get length
	std::string length_str {s.recv(comms_constants::LENGTH_LENGTH)};
	unsigned long length;
	std::memcpy(&length, length_str.data(), comms_constants::LENGTH_LENGTH);

	// get content
	for (int i = 0 ; i < length / comms_constants::BATCH_SIZE ; i++)
		msg.content += s.recv(comms_constants::BATCH_SIZE);
	msg.content += s.recv(length % comms_constants::BATCH_SIZE);

	return msg;
}
