#include <algorithm>
#include <cstdint>
#include <hsql/SQLParser.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <vector>
#include "communication_protocol.h"
#include "table/table.h"
#include "table/table_storage_constants.h"
#include "tables_loader.h"
#include "client_handler.h"
#include <sys/sendfile.h>
#include "utils.h"

using namespace hsql;

void ClientThread::handle_select_statement(const SelectStatement* statement)
{ 
	using std::string_literals::operator""s;

	const Table& source_table = TablesLoader::get_instance().get_table(statement->fromTable->getName());

	// queried columns
	std::vector<Column> result_columns;
	for (const auto& col_ptr : *statement->selectList)
	{
		try {
			result_columns.push_back(source_table.get_column(col_ptr->getName()));
		} catch (no_such_column &e) { 
			std::cerr << e.what() << std::endl;
		}
	}

	// now we have required columns listed. create new result temporary table.
	std::string temp_table_name {(std::stringstream() << "tmp/tmp_" << std::this_thread::get_id()).str()};

	Table res_table {temp_table_name, result_columns};

	for (rows_count_t i = 0 ; i < source_table.get_rows_count() ; i++)
	{
		for (const Column& c : result_columns)
		{
			res_table.set_cell(i, c, source_table.get_cell(i, c).get());
		}
	}

	// send message with only the result char as content, the rest will be sent with send_file
	comms::message_t msg;
	msg.command = comms_constants::CMD_QUERY_RESULT;
	msg.content = comms_constants::QUERY_RES_SUCCESS + res_table.get_file_data();

	comms::send_message(m_client, msg);
}

/* handle a query from the client */
void ClientThread::handle_query(const std::string& query)
{
	SQLParserResult parsing_result;

	// if query is illegal send error..
	if (!SQLParser::parse(query, &parsing_result)) 
	{
		comms::message_t msg;
		msg.command = comms_constants::CMD_QUERY_RESULT;
		msg.content = comms_constants::QUERY_RES_ERROR + parsing_result.errorMsg();

		comms::send_message(m_client, msg);
		return;	
	};

	for (const SQLStatement* statement : parsing_result.getStatements())
	{
		// handle kinds of statements
		switch (statement->type()) {
			case hsql::kStmtSelect:
				handle_select_statement(static_cast<const SelectStatement*>(statement));
				break;
			default:
				std::cerr << "feature not implemented yet!" << std::endl;
		}
	}
}
