#include <atomic>
#include <sys/poll.h>
#include <openssl/ssl.h>

#include <spdlog/spdlog.h>
#include <unistd.h>

#include "client_handler.h"
#include "exceptions.h"
#include "utils.h"

std::atomic<bool> ClientThread::program_running = true;

/* on constructor call, start thread */
ClientThread::ClientThread(SSL* client_ssl) : 
	client_ssl(client_ssl)
{
}

/* move ctor. no deallocation of resources */
ClientThread::ClientThread(ClientThread&& cli) :
	client_ssl(cli.client_ssl)

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
			comms::send_message(client_ssl, JOIN_SUCCESS_MESSAGE);
			spdlog::get("server")->info("client joined. confirmed.");
			break;

		case CMD_LEAVE:
			spdlog::get("server")->info("client left");
			return true;
			 
		case CMD_QUERY:
			try { handle_query(msg.content); }
			catch (beek_exception& e)
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
    finalize();
}

void ClientThread::finalize()
{
    SSL_shutdown(client_ssl);
    SSL_free(client_ssl);
}
