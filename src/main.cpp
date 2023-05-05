#include <cstdlib>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <spdlog/common.h>
#include <sys/socket.h>
#include <vector>
#include <thread>
#include <csignal>
#include <atomic>

#include <spdlog/spdlog.h>
#include <openssl/ssl.h>

#include "communication_protocol.h"
#include "client_handler.h"
#include "table/table.h"
#include "table/types.h"
#include "tables_loader.h"

int create_socket()
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(1337);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		spdlog::error("socket creation error");

	if (bind(sock, (struct sockaddr*)&addr, sizeof addr) < 0 ) 
		spdlog::error("socket bind error");

	if (listen(sock, 1))
		spdlog::error("socket listen error");

	return sock;
}

/**
 * handle ctrl+c (sigint) to close and cleanup the threads.
 * callback function.
 */
void handle_sigint(int dummy)
{
	// tell threads to stop
	ClientThread::program_running = false;

	// wait for them all to join
	for (auto&& ct: ClientThread::running_client_threads) {
		ct->join();
	}

	exit(EXIT_SUCCESS);
}

void setup_logger()
{
	spdlog::set_level(spdlog::level::debug);
	spdlog::set_pattern("[thread %t] %v");
}

int main()
{
	setup_logger();

	SSL_CTX* ssl_context = SSL_CTX_new(TLS_server_method());

	server.bind(1337);
	server.listen(2);

	signal(SIGINT, handle_sigint);

	spdlog::info("listening to connections..");

	// repeatedly accept clients, handle them in seperate threads.
	while (true)
	{
		Socket client{server.accept()};

		// add new ClientThread to the running threads (with the socket)
		ClientThread::running_client_threads.push_back(std::make_unique<ClientThread>(client));
	}
}
