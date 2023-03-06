
#include <atomic>
#include <thread>
#include <vector>
#include "cpp_socket.h"

class ClientThread
{
	private:
		Socket m_client;
		std::thread m_thread;
		
		/* thread execution entry point */
		void run();

	public:
		/* is false when the program should be terminated */
		static std::atomic<bool> program_running;

		/* vector of currently running client threads */
		static std::vector<ClientThread> running_client_threads;

		void join()
		{
			m_thread.join();
		}

		ClientThread(Socket client_socket);
};
