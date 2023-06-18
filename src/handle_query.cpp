#include <hsql/sql/Expr.h>
#include <spdlog/spdlog.h>

#include "client_handler.h"
#include "table/table.h"
#include "communication_protocol.h"
#include "exceptions.h"
#include "table/types.h"
#include "tables_loader.h"
#include "utils.h"
#include "where_clause.h"

using namespace comms_constants;

void ClientThread::handle_select_statement(const hsql::SelectStatement* statement)
{ 
	const Table* source_table;

	if (statement->fromTable == NULL) throw no_such_table("no table given");
    
	source_table = &TablesLoader::get_instance().get_table(statement->fromTable->getName());

	spdlog::get("handle")->info("target table is: '{}'", source_table->get_name());

	// queried columns
	std::vector<Column> result_columns;
	for (const auto& col_ptr : *statement->selectList)
	{
		// check star case 
		if (col_ptr->isType(hsql::kExprStar))
			result_columns.insert(result_columns.end(), source_table->get_columns().begin(), source_table->get_columns().end());
		else
			result_columns.push_back(source_table->get_column(col_ptr->getName()));
	}

	spdlog::get("handle")->info("requested columns: ");
	for (auto c : result_columns)
		spdlog::get("handle")->info("{}", c.get_name());

	// now we have required columns listed. create new result temporary table.
	fs::create_directory(table_storage::TEMP_DIR);
	std::string temp_table_path {table_storage::TEMP_DIR / ("tmp" + get_thread_id())};

	create_table(temp_table_path, result_columns);

	Table res_table(temp_table_path);

	// values into temp table
	source_table->for_each([&](Record&& r) {

        // should I? (where clause)
        if (statement->whereClause != NULL) {
            auto where_res = eval(statement->whereClause, r);

            if (where_res->type != hsql::kExprLiteralInt)
                throw where_clause_error("error in evaluating where clause.");

            if (!where_res->ival)
                return; // skip since not needed
        }

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
	send_query_result(client_ssl, true, res_table.get_file_data());
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
			spdlog::get("handle")->error("varchar length must be {} currently", table_storage::VARCHAR_50_SIZE);
		}

		columns.emplace_back(df->name, SQL_TYPE_TO_COLUMN_TYPE.at(df->type.data_type));
	}

	// actually create the table
	create_table(table_storage::TABLES_DIR / statement->tableName, columns);
	
	// reload tables for loader
	TablesLoader::get_instance().reload_tables();

	// response
	send_query_result(client_ssl, true, "");
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
					throw incorrect_type::classic(column.get_name());

				new_record.put(column.get_name(), IntegerValue(value->ival));
				break;

			case REAL:
				if (value->isType(hsql::kExprLiteralInt))
					new_record.put(column.get_name(), RealValue(value->ival));
				else if (value->isType(hsql::kExprLiteralFloat))
					new_record.put(column.get_name(), RealValue(value->fval));
				else
					throw incorrect_type::classic(column.get_name());
				break;

			case VARCHAR_50:
				if (!value->isType(hsql::kExprLiteralString))
					throw incorrect_type::classic(column.get_name());

				new_record.put(column.get_name(), VarChar50Value(value->getName()));
				break;
		}
	}

	dest_table.insert(new_record);

	send_query_result(client_ssl, true, "");
}

/* handle the update statement */
void ClientThread::handle_update_statement(const hsql::UpdateStatement* statement)
{
    Table& dest_table = TablesLoader::get_instance().get_table(statement->table->getName());

    // check columns literals in case something's wrong. convert int literals to float literals where appropriate.
    for (hsql::UpdateClause* col_val : *statement->updates) 
    {
        const Column& col = dest_table.get_column(col_val->column);

        switch (col.get_type()) {
            case INTEGER:
                if (!col_val->value->isType(hsql::kExprLiteralInt))
                    throw incorrect_type::classic(col.get_name());
                break;
            case REAL:
                if (col_val->value->isType(hsql::kExprLiteralInt)) 
                    col_val->value = Expr::makeLiteral(static_cast<double>(col_val->value->ival));
                else if (!col_val->value->isType(hsql::kExprLiteralFloat))
                    throw incorrect_type::classic(col.get_name());
                break;
            case VARCHAR_50:
                if (!col_val->value->isType(hsql::kExprLiteralString))
                    throw incorrect_type::classic(col.get_name());
                break;
        }
    }
 
    dest_table.for_each([&](Record&& r) {

            // should I? (where clause)
            if (statement->where != NULL) {
                auto where_res = eval(statement->where, r);

                if (where_res->type != hsql::kExprLiteralInt)
                    throw where_clause_error("error in evaluating where clause.");

                if (!where_res->ival)
                    return; // skip since not needed
            }

            for (auto col_val : *statement->updates) 
            {
                const Column& col = dest_table.get_column(col_val->column);

                switch (col.get_type()) {
                    case INTEGER:
                        r.put(col.get_name(), IntegerValue(col_val->value->ival));
                        break;

                    case REAL:
                        r.put(col.get_name(), RealValue(col_val->value->fval));
                        break;

                    case VARCHAR_50:
                        r.put(col.get_name(), VarChar50Value(col_val->value->getName()));
                        break;
                }
            }

            r.update();
    });

    send_query_result(client_ssl, true, "");
}

/* handle a query from the client */
void ClientThread::handle_query(const std::string& query)
{
	using namespace hsql;

	spdlog::get("handle")->info("handling query: '{}'", query);
	SQLParserResult parsing_result;

	// if query is illegal send error..
	SQLParser::parse(query, &parsing_result);

	if (!parsing_result.isValid()) 
	{
		comms::message_t msg(comms_constants::CMD_QUERY_RESULT, comms_constants::QUERY_RES_ERROR + parsing_result.errorMsg());

		comms::send_message(client_ssl, msg);
		return;	
	};

	spdlog::get("handle")->info("query is valid!");

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

            case hsql::kStmtUpdate:
                handle_update_statement(static_cast<const UpdateStatement*>(statement));
                break;

			default:
                throw not_implemented("feature not implelmented yet..");
		}
	}
}

