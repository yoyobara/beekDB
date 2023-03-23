#include <algorithm>
#include <hsql/SQLParser.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "table/table.h"
#include "tables_loader.h"
#include "client_handler.h"

using namespace hsql;
namespace fs = std::filesystem;

void handle_select_statement(const SelectStatement* statement)
{ 
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

	// now we have required columns listed. create new result table.
	std::stringstream res_table_name;
	res_table_name << "tmp/tmp_" << std::this_thread::get_id();
	Table res_table {res_table_name.str(), result_columns};

	for (rows_count_t i = 0 ; i < source_table.get_rows_count() ; i++)
	{
		for (const Column& c : result_columns)
		{
			res_table.set_cell(i, c, source_table.get_cell(i, c).get());
		}
	}
	std::cout << "wrote to table" << std::endl;
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
