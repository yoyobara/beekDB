#include <algorithm>
#include <cstdint>
#include <hsql/SQLParser.h>
#include <filesystem>
#include <hsql/sql/ColumnType.h>
#include <hsql/sql/CreateStatement.h>
#include <hsql/sql/Expr.h>
#include <hsql/sql/InsertStatement.h>
#include <hsql/sql/SQLStatement.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <vector>
#include "communication_protocol.h"
#include "table/table.h"
#include "table/table_storage_constants.h"
#include "table/types.h"
#include "tables_loader.h"
#include "client_handler.h"
#include <sys/sendfile.h>
#include "utils.h"

using namespace comms_constants;

void ClientThread::handle_select_statement(const hsql::SelectStatement* statement)
{ 
	const Table& source_table = TablesLoader::get_instance().get_table(table_storage::TABLES_DIR / statement->fromTable->getName());

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
	fs::create_directory(table_storage::TEMP_DIR);
	std::string temp_table_path {table_storage::TEMP_DIR / ("tmp" + get_thread_id())};

	create_table(result_columns, temp_table_path);

	Table res_table(temp_table_path);

	for (long i = 0 ; i < source_table.get_rows_count() ; i++)
	{
		for (const Column& c : result_columns)
		{
			res_table.set_cell(i, c, source_table.get_cell(i, c).get());
		}
	}

	// send message with only the result char as content, the rest will be sent with send_file
	comms::send_message(m_client, comms::message_t(comms_constants::CMD_QUERY_RESULT, comms_constants::QUERY_RES_SUCCESS + res_table.get_file_data()));
}

void ClientThread::handle_create_statement(const hsql::CreateStatement* statement)
{
	static const std::map<hsql::DataType, ColumnType> SQL_TYPE_TO_COLUMN_TYPE {
		{hsql::DataType::INT, INTEGER},
		{hsql::DataType::VARCHAR, VARCHAR_50},
		{hsql::DataType::REAL, REAL}
	};

	// table creation
	std::vector<Column> columns;

	// created table columns

	for (hsql::ColumnDefinition* df : *statement->columns)
	{
		if (df->type.data_type == hsql::DataType::VARCHAR && df->type.length != table_storage::VARCHAR_50_SIZE)
		{
			spdlog::error("varchar length must be {} currently", table_storage::VARCHAR_50_SIZE);
		}

		columns.emplace_back(df->name, SQL_TYPE_TO_COLUMN_TYPE.at(df->type.data_type));
	}

	// actually create the table
	create_table(columns, table_storage::TABLES_DIR / statement->tableName);
}

void ClientThread::handle_insert_statement(const hsql::InsertStatement* statement)
{
	Table& dest_table = TablesLoader::get_instance().get_table(statement->tableName);

	const uint64_t row {dest_table.get_rows_count() + 1};

	for (int i = 0 ; i < statement->columns->size() ; i++)
	{
		const Column& column = dest_table.get_column(statement->columns->at(i));
		hsql::Expr* value = statement->values->at(i);

		switch (value->type) {
			case hsql::kExprLiteralInt: {
				IntegerValue ival(value->ival);
				dest_table.set_cell(row, column, &ival);
				break;
			}

			case hsql::kExprLiteralFloat: {
				RealValue rval(value->fval);
				dest_table.set_cell(row, column, &rval);
				break;
			}

			case hsql::kExprLiteralString: {
				VarChar50Value vval(value->name);
				dest_table.set_cell(row, column, &vval);
		    }

			default:
				spdlog::error("feature not implemented yet..");
		}
	}

	comms::send_message(m_client, comms::message_t(CMD_QUERY_RESULT, std::to_string(QUERY_RES_SUCCESS)));
}

/* handle a query from the client */
void ClientThread::handle_query(const std::string& query)
{
	using namespace hsql;

	spdlog::info("handling query: '{}'", query);
	SQLParserResult parsing_result;

	// if query is illegal send error..
	SQLParser::parse(query, &parsing_result);

	if (!parsing_result.isValid()) 
	{
		comms::message_t msg(comms_constants::CMD_QUERY_RESULT, comms_constants::QUERY_RES_ERROR + parsing_result.errorMsg());

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
			
			case hsql::kStmtCreate:
				handle_create_statement(static_cast<const CreateStatement*>(statement));
				break;

			case hsql::kStmtInsert:
				handle_insert_statement(static_cast<const InsertStatement*>(statement)); 
				break;

			default:
				spdlog::error("feature not implemented yet..");
		}
	}
}
