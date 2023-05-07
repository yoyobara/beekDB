#include <memory>
#include <spdlog/spdlog.h>
#include <sys/poll.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include "communication_protocol.h"
#include "client_handler.h"

std::vector<std::unique_ptr<ClientThread>> ClientThread::running_client_threads;
std::atomic<bool> ClientThread::program_running = true;

/* on constructor call, start thread */
ClientThread::ClientThread(int client_socket_descriptor) : 
	m_client_descriptor(client_socket_descriptor), 
	m_is_joined(false)
{
	spdlog::info("new client thread with fd={}", m_client_descriptor);
}

/* move */
ClientThread::ClientThread(ClientThread&& moved) : 
	m_client_descriptor(moved.m_client_descriptor),
	m_is_joined(false)
{
	spdlog::debug("moved client thread");
}
/* checks whether a message is ready to be read from the client.
 * this enables checking for program interruption. (not freezing)
 */
bool ClientThread::is_message_waiting()
{
	pollfd fds{m_client_descriptor, POLLIN};
	int pollres { poll(&fds, 1, 500) };

	if (pollres < 0) spdlog::error("poll error");

	return pollres;
}

/*
 * handle a message.
 * returns whether the client left the server.
 */
bool ClientThread::process_message(comms::message_t&& msg)
{
	using namespace comms_constants;

	switch (msg.command) {
		case CMD_JOIN:
			this->m_is_joined = true;
			comms::send_message(m_client, JOIN_SUCCESS_MESSAGE);
			spdlog::info("client joined. confirmed.");
			break;

		case CMD_LEAVE:
			spdlog::info("client left");
			return true;
			 
		case CMD_QUERY:
			handle_query(msg.content);
	}

	return false;
}

/*
 * run enrty point for a single client handler. will check if can read a message periodically.
 */
void ClientThread::operator()()
{
	while (ClientThread::program_running) {

		// is_message_waiting shall block for some time
		if (is_message_waiting() && process_message(comms::recv_message(m_client)))
			break;

		spdlog::debug("my fd is {}", m_client_descriptor);
	}

	std::cout << "called close\n";
	close(m_client_descriptor);
}
