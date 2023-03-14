#include <sys/poll.h>
#include <iostream>
#include "communication_protocol.h"
#include "client_handler.h"

std::vector<ClientThread> ClientThread::running_client_threads;
std::atomic<bool> ClientThread::program_running = true;

/* on constructor call, start thread */
ClientThread::ClientThread(Socket client_socket) : 
	m_client(client_socket), 
	m_thread(&ClientThread::run, this),
	m_is_joined(false)
{
}

void ClientThread::join()
{
	this->m_thread.join();
}

/* checks whether a message is ready to be read from the client.
 * this enables checking for program interruption. (not freezing)
 */
bool ClientThread::is_message_waiting()
{
	pollfd fds{m_client.get_fd(), POLLIN};
	int pollres { poll(&fds, 1, 500) };

	if (pollres < 0) throw socket_error("poll error");

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
			break;

		case CMD_LEAVE:
			return true;
			 
		case CMD_QUERY:
			handle_query(msg.content);
	}

	return false;
}

/*
 * run enrty point for a single client handler. will check if can read a message periodically.
 */
void ClientThread::run()
{
	while (ClientThread::program_running) {

		// is_message_waiting shall block for some time
		if (is_message_waiting())
			process_message(comms::recv_message(m_client));
	}

	m_client.close();
}
