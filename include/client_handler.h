#pragma once

#include <atomic>
#include <hsql/sql/CreateStatement.h>
#include <hsql/sql/InsertStatement.h>
#include <memory>
#include <openssl/types.h>
#include <thread>
#include <vector>
#include "communication_protocol.h"
#include "hsql/sql/SelectStatement.h"

class ClientThread
{
	private:
		SSL* client_ssl;
		bool m_is_joined;

		bool is_message_waiting();
		
		bool process_message(comms::message_t&& msg);

		void handle_query(const std::string& query);

		void handle_select_statement(const hsql::SelectStatement* statement);
		void handle_create_statement(const hsql::CreateStatement* statement);
		void handle_insert_statement(const hsql::InsertStatement* statement);

	public:
		/* is false when the program should be terminated */
		static std::atomic<bool> program_running;

		void join();

		ClientThread(SSL* client_ssl);
		ClientThread(ClientThread&& cli);

		~ClientThread();

		/* thread execution entry point */
		void operator()();
};
