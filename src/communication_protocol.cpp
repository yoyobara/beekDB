#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>

#include "communication_protocol.h"
#include "utils.h"

void comms::send_message(SSL* ssl, const message_t& message) // length is -1 by default
{
	spdlog::get("network")->info("sending message (command: '{}', content: '{:n}', content_size: {} bytes)", message.command, spdlog::to_hex(message.content.begin(), message.content.end()), message.content.size());

	ssl_utils::write_s(ssl, message.command + encode(message.content.size()) + message.content);

	spdlog::get("network")->info("sent");
}

comms::message_t comms::recv_message(SSL* ssl)
{
	spdlog::get("network")->info("recieving message");

	comms::message_t msg;
	msg.command = ssl_utils::read_s(ssl, comms_constants::CMD_LENGTH)[0];

	// get length
	std::string length_str {ssl_utils::read_s(ssl, comms_constants::LENGTH_LENGTH)};
	uint64_t length;
	std::memcpy(&length, length_str.data(), comms_constants::LENGTH_LENGTH);

	spdlog::get("network")->info("length is {} bytes", length);

	// get content
	if (length != 0)
	{
		for (int i = 0 ; i < length / comms_constants::BATCH_SIZE ; i++)
			msg.content += ssl_utils::read_s(ssl, comms_constants::BATCH_SIZE);
		msg.content += ssl_utils::read_s(ssl, length % comms_constants::BATCH_SIZE);
	}

	spdlog::get("network")->info("recieved message (command: '{}', content: '{}', content_size: {} bytes)", msg.command, spdlog::to_hex(msg.content.begin(), msg.content.end()), msg.content.size());

	return msg;
}
