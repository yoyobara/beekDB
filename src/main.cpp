#include <cstdlib>
#include <iostream>
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
	for (ClientThread& ct: ClientThread::running_client_threads) {
		ct.join();
	}

	exit(EXIT_SUCCESS);
}

void setup_logger()
{
	spdlog::set_level(spdlog::level::debug);
}

int main()
{
	setup_logger();

	Socket server;
	server.bind(1337);
	server.listen(2);

	signal(SIGINT, handle_sigint);

	spdlog::info("listening to connections..");

	std::cout << "start\n";
	for (const Record& r : TablesLoader::get_instance().get_table("tabs"))
	{
		Column c("n", INTEGER);
		std::cout << r.get<IntegerValue>(c).int_val << std::endl;
	}
	std::cout << "end\n";

	// repeatedly accept clients, handle them in seperate threads.
	while (true)
	{
		Socket client = server.accept();

		// add new ClientThread to the running threads (with the socket)
		ClientThread::running_client_threads.emplace_back(client);
	}
}
