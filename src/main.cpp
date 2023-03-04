#include <iostream>
#include <vector>
#include <thread>

#include "cpp_socket.h"
#include "table.h"

/**
 * a thread running function which is in charge of handling a simple client
 */
void handle_client(Socket client)
{
	client.close();
}

int main()
{
	Socket server;
	server.bind(1337);
	server.listen(2);

	// a vector containing the currently running threads
	std::vector<std::thread> running_threads;

	// repeatedly accept clients, handle them in seperate threads.
	while (true)
	{
		Socket client = server.accept();
		running_threads.push_back(std::thread(handle_client, client));
	}

	return EXIT_SUCCESS;
}
