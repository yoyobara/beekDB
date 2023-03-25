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

	// now send response. manual send since we need to send a file's content.
	uint64_t table_size = res_table.evaluate_size();
	send(m_client.get_fd(), (comms_constants::CMD_QUERY_RESULT + encode(table_size)).c_str(), comms_constants::LENGTH_LENGTH + comms_constants::CMD_LENGTH, 0);
}

/* handle a query from the client */
bool ClientThread::handle_query(const std::string& query)
{
	SQLParserResult parsing_result;
	if (!SQLParser::parse(query, &parsing_result)) return false;

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

	return true;
}
