#include <sys/poll.h>
#include <iostream>
#include "communication_protocol.h"
#include "client_handler.h"

std::vector<ClientThread> ClientThread::running_client_threads;
std::atomic<bool> ClientThread::program_running = true;

/* on constructor call, start thread */
ClientThread::ClientThread(Socket client_socket) : 
	m_client(client_socket), 
	m_thread(&ClientThread::run, this)
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
 * handle a message
 */
void process_message(Socket& s, comms::message_t&& msg)
{
}

/*
 * run enrty point for a single client handler. will check if can read a message periodically.
 */
void ClientThread::run()
{
	while (ClientThread::program_running) {

		// is_message_waiting shall block for some time
		if (is_message_waiting())
			process_message(m_client, comms::recv_message(m_client));
	}

	m_client.close();
}
