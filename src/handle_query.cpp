#include <hsql/SQLParser.h>
#include <hsql/SQLParserResult.h>
#include <hsql/sql/SQLStatement.h>
#include <hsql/sql/SelectStatement.h>
#include <iostream>
#include <filesystem>
#include "client_handler.h"
#include "table.h"

using namespace hsql;
namespace fs = std::filesystem;

void handle_select_statement(const SelectStatement* statement) { }

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
			default:
				std::cerr << "feature not implemented yet!" << std::endl;
		}
	}

	return true;
}
