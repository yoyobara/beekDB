#pragma once

#include <atomic>
#include <thread>
#include <vector>
#include "communication_protocol.h"
#include "cpp_socket.h"
#include "hsql/sql/SelectStatement.h"

class ClientThread
{
	private:
		Socket m_client;
		std::thread m_thread;
		bool m_is_joined;

		bool is_message_waiting();
		
		bool process_message(comms::message_t&& msg);

		bool handle_query(const std::string& query);

		void handle_select_statement(const hsql::SelectStatement* statement);

		/* thread execution entry point */
		void run();

	public:
		/* is false when the program should be terminated */
		static std::atomic<bool> program_running;

		/* vector of currently running client threads */
		static std::vector<ClientThread> running_client_threads;

		void join();

		ClientThread(Socket client_socket);
};
