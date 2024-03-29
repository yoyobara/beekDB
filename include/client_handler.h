#pragma once

#include <hsql/SQLParser.h>

#include "communication_protocol.h"

class ClientThread
{
	private:
		SSL* client_ssl;

		bool is_message_waiting();
		
		bool process_message(comms::message_t&& msg);

		void handle_query(const std::string& query);

		void handle_select_statement(const hsql::SelectStatement* statement);
		void handle_create_statement(const hsql::CreateStatement* statement);
		void handle_insert_statement(const hsql::InsertStatement* statement);
        void handle_update_statement(const hsql::UpdateStatement* statement);

        // cleanup ssl and stuff
        void finalize();

	public:
		/* is false when the program should be terminated */
		static std::atomic<bool> program_running;

		ClientThread(SSL* client_ssl);
		ClientThread(ClientThread&& cli);

		/* thread execution entry point */
		void operator()();
};
