#pragma once

#include <atomic>
#include <hsql/sql/CreateStatement.h>
#include <hsql/sql/InsertStatement.h>
#include <memory>
#include <thread>
#include <vector>
#include "communication_protocol.h"
#include "cpp_socket.h"
#include "hsql/sql/SelectStatement.h"
#include "cryptopp/dh.h"
#include "cryptopp/osrng.h"

class ClientThread
{
	public:
		Socket m_client;
		std::thread m_thread;
		bool m_is_joined;

		/* random generator for encrypted traffic */
		CryptoPP::AutoSeededRandomPool rnd_gen;

		bool is_message_waiting();
		
		bool process_message(comms::message_t&& msg);

		void handle_client_join(std::string&& message_content);
		void handle_query(const std::string& query);

		void handle_select_statement(const hsql::SelectStatement* statement);
		void handle_create_statement(const hsql::CreateStatement* statement);
		void handle_insert_statement(const hsql::InsertStatement* statement);

		/* thread execution entry point */
		void run();

	public:
		/* is false when the program should be terminated */
		static std::atomic<bool> program_running;

		/* vector of currently running client threads */
		static std::vector<std::unique_ptr<ClientThread>> running_client_threads;

		void join_thread();

		ClientThread(Socket client_socket);

		ClientThread(ClientThread&& moved);
};
