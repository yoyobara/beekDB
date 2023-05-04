#include <cstdlib>
#include <iostream>
#include <memory>
#include <spdlog/common.h>
#include <vector>
#include <thread>
#include <csignal>
#include <atomic>

#include <spdlog/spdlog.h>

#include "cpp_socket.h"
#include "communication_protocol.h"
#include "client_handler.h"
#include "table/table.h"
#include "table/types.h"
#include "tables_loader.h"

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
		ct->join_thread();
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

	Socket server;
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
