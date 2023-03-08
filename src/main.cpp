#include <cstdlib>
#include <iostream>
#include <vector>
#include <thread>
#include <csignal>
#include <atomic>

#include "cpp_socket.h"
#include "communication_protocol.h"
#include "table.h"
#include "client_handler.h"

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

int main()
{
	Socket server;
	server.bind(1337);
	server.listen(2);

	signal(SIGINT, handle_sigint);

	// repeatedly accept clients, handle them in seperate threads.
	while (true)
	{
		Socket client = server.accept();
		
		// add new ClientThread to the running threads (with the socket)
		ClientThread::running_client_threads.emplace_back(client);
	}
}
