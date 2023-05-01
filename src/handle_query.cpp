#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <hsql/SQLParser.h>
#include <filesystem>
#include <hsql/sql/ColumnType.h>
#include <hsql/sql/CreateStatement.h>
#include <hsql/sql/Expr.h>
#include <hsql/sql/InsertStatement.h>
#include <hsql/sql/SQLStatement.h>
#include <iostream>
#include <memory>
#include <spdlog/fmt/bin_to_hex.h>
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
	const Table* source_table;
	try {
		source_table = &TablesLoader::get_instance().get_table(statement->fromTable->getName());
	} catch (no_such_table& e)
	{
	comms::send_message(m_client, comms::message_t(comms_constants::CMD_QUERY_RESULT, comms_constants::QUERY_RES_ERROR + e.msg));
	return;
	}

	// queried columns
	std::vector<Column> result_columns;
	for (const auto& col_ptr : *statement->selectList)
	{
		try {

			// check star case 
			if (col_ptr->isType(hsql::kExprStar))
				result_columns.insert(result_columns.end(), source_table.get_columns().begin(), source_table.get_columns().end());
			else
				result_columns.push_back(source_table->get_column(col_ptr->getName()));

		} catch (no_such_column &e) { 
			std::cerr << e.what() << std::endl;
		}
	}

	// now we have required columns listed. create new result temporary table.
	fs::create_directory(table_storage::TEMP_DIR);
	std::string temp_table_path {table_storage::TEMP_DIR / ("tmp" + get_thread_id())};

	spdlog::debug("here");
	create_table(temp_table_path, result_columns);
	spdlog::debug("not");

	Table res_table(temp_table_path);

	// values into temp table
	source_table.for_each([&](Record&& r) {
		Record new_record(&res_table);
		for (const Column& col : result_columns)
		{
			switch (col.get_type()) {
				case INTEGER:
					new_record.put(col.get_name(), r.get<IntegerValue>(col.get_name()));
					break;
				case REAL:
					new_record.put(col.get_name(), r.get<RealValue>(col.get_name()));
					break;
				case VARCHAR_50:
					new_record.put(col.get_name(), r.get<VarChar50Value>(col.get_name()));
					break;
			}
		}

		res_table.insert(new_record);
	});

	// send table response
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
	create_table(table_storage::TABLES_DIR / statement->tableName, columns);
	
	// reload tables for loader
	TablesLoader::get_instance().reload_tables();

	// response
	comms::send_message(m_client, QUERY_RESULT_SUCCESS_NO_CONTENT);
}

void ClientThread::handle_insert_statement(const hsql::InsertStatement* statement)
{
	Table& dest_table = TablesLoader::get_instance().get_table(statement->tableName);
	std::vector<Column> selected_columns;

	// if no column list is provided, use all columns, elsewhere use the specified columns
	if (statement->columns == nullptr)
		selected_columns = dest_table.get_columns();
	else
		for (const char* ex : *statement->columns)
			selected_columns.push_back(dest_table.get_column(ex));

	Record new_record(&dest_table);
	for (int i = 0 ; i < selected_columns.size() ; i++)
	{
		hsql::Expr* value {statement->values->at(i)};
		const Column& column {selected_columns.at(i)};

		switch (column.get_type()) {
			case INTEGER:
				if (!value->isType(hsql::kExprLiteralInt)) 
					spdlog::error("incorrect literal type for column {}", column.get_name());

				new_record.put(column.get_name(), IntegerValue(value->ival));
				break;

			case REAL:
				if (value->isType(hsql::kExprLiteralInt))
					new_record.put(column.get_name(), RealValue(value->ival));
				else if (value->isType(hsql::kExprLiteralFloat))
					new_record.put(column.get_name(), RealValue(value->fval));
				else
					spdlog::error("incorrect literal type for column {}", column.get_name());
				break;

			case VARCHAR_50:
				if (!value->isType(hsql::kExprLiteralString))
					spdlog::error("incorrect literal type for column {}", column.get_name());

				new_record.put(column.get_name(), VarChar50Value(value->getName()));
				break;
		}
	}

	dest_table.insert(new_record);

	comms::send_message(m_client, QUERY_RESULT_SUCCESS_NO_CONTENT);
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

