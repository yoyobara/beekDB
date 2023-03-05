#include <cstdlib>
#include <iostream>
#include <vector>
#include <thread>
#include <csignal>
#include <atomic>

#include "cpp_socket.h"
#include "communication_protocol.h"
#include "table.h"

// a vector containing the currently running threads
std::vector<std::thread> running_threads;
std::atomic<bool> stop_program(false);

class ClientThread
{
	private:

		std::thread m_thread;
		Socket m_client;

		void run()
		{
			// TODO run program
		}

		void stop()
		{
			// TODO close connections and cleanup
			m_thread.join();
		}

	public:

		ClientThread(Socket client) : m_client(client) {}

		void start()
		{
			m_thread = std::thread(&ClientThread::run, this);
		}
};

/**
 * handle ctrl+c (sigint) to close and cleanup the threads
 */
void handle_sigint(int dummy)
{
	stop_program = true;

	for (std::thread& t: running_threads) {
		t.join();
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
		running_threads.push_back(std::thread(handle_client, client));
	}
}
