#include <memory>
#include <openssl/ssl.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/spdlog.h>
#include <sys/poll.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include "communication_protocol.h"
#include "client_handler.h"
#include "logging.h"
#include "tables_loader.h"
#include "utils.h"

std::atomic<bool> ClientThread::program_running = true;

/* on constructor call, start thread */
ClientThread::ClientThread(SSL* client_ssl) : 
	client_ssl(client_ssl),
	m_is_joined(false)
{
}

/* move ctor. no deallocation of resources */
ClientThread::ClientThread(ClientThread&& cli) :
	client_ssl(cli.client_ssl),
	m_is_joined(cli.m_is_joined)

{
	cli.client_ssl = nullptr;
}

/* checks whether a message is ready to be read from the client.
 * this enables checking for program interruption. (not freezing)
 */
bool ClientThread::is_message_waiting()
{
	pollfd fds{SSL_get_fd(client_ssl), POLLIN};
	int pollres { poll(&fds, 1, 500) };

	if (pollres < 0) spdlog::get("server")->error("poll error");

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
			comms::send_message(client_ssl, JOIN_SUCCESS_MESSAGE);
			spdlog::get("server")->info("client joined. confirmed.");
			break;

		case CMD_LEAVE:
			spdlog::get("server")->info("client left");
			return true;
			 
		case CMD_QUERY:
			try { handle_query(msg.content); }
			catch (std::exception& e)
			{
				// in case of failure, an exception is thrown and the appropriate response is sent and logged
				spdlog::get("handle")->error("{}", e.what());
				send_query_result(client_ssl, false, e.what());
			}
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
		if (is_message_waiting() && process_message(comms::recv_message(client_ssl)))
			break;

	}
}

ClientThread::~ClientThread()
{
	SSL_free(client_ssl);
}
