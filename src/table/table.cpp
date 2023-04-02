#include <algorithm>
#include <array>
#include <ios>
#include <memory>
#include <numeric>
#include <sstream>
#include <iostream>
#include <string>

#include "table/table.h"
#include "table/table_storage_constants.h"
#include "table/types.h"
#include "utils.h"

using namespace table_storage;

/* column */

std::ostream& operator<<(std::ostream& out, const Column& c)
{
	out << c.get_name() << ": " << c.get_type();
	return out;
}


/* table */

/* open */
void Table::init_columns()
{
	// now cursor at start of columns.

	char buffer;

	for (int i = 0 ; i < m_columns_count ; i++)
	{
		// read descriptor
		table_file.read(&buffer, DESC_SIZE);

		ColumnType type {BYTE_TO_TYPE.at(buffer)};

		// read name
		std::string col_name;
		do {
			table_file.read(&buffer, 1);
			col_name += buffer;
		}
		while (buffer != '\0');
		col_name.pop_back(); // \0

		// add column
		m_columns.push_back(Column(col_name, type));
	}
}

void Table::init_metadata()
{
	// assert signature presence
	assert(table_file.verify_content(SIGNATURE_OFFSET, table_storage::SIGNATURE));

	// read columns count from metadata
	table_file.read_at(COLUMN_COUNT_OFFSET, &m_columns_count, sizeof m_columns_count);

	// read rows count from metadata.
	table_file.read_at(ROW_COUNT_OFFSET, &m_rows_count, sizeof m_rows_count);

	init_columns();

	// now metadata is over, at start of table itself
	m_table_start = table_file.tellg();
}

/* open */
// TODO remove hardcoded paths (switch to fs something)
Table::Table(const std::string& name) :
	table_file(table_storage::TABLES_DIR + "/" + name, false),
	m_name(name)
{
	init_metadata();
	init_row_size();
}

void Table::init_row_size()
{
	m_row_size = std::accumulate(m_columns.begin(), m_columns.end(), 0, [](int current, const Column& next){ return current + next.get_size();});
}

/* get cell */
std::unique_ptr<TableValue> Table::get_cell(rows_count_t row_index, const Column &column) const
{
	// cell offset
	uint64_t offset = m_table_start + row_index * m_row_size;
	for (const Column& c : m_columns)
	{
		if (c == column)
			break;
		offset += c.get_size();
	}

	switch (column.get_type()) {
		case INTEGER:
			int i;
			table_file.read_at(offset, &i, TYPE_SIZE.at(INTEGER));

			return std::make_unique<IntegerValue>(i);

		case REAL:
			double d;
			table_file.read_at(offset, &d, TYPE_SIZE.at(REAL));

			return std::make_unique<RealValue>(d);

		case STRING:
			std::array<char, STRING_SIZE> buff;
			table_file.read_at(offset, buff.data(), TYPE_SIZE.at(STRING));

			return std::make_unique<StringValue>(buff.data());

		default:
			return nullptr; // SHOULD NOT REACH THIS
	}
}


void Table::set_cell(rows_count_t row_index, const Column& column, TableValue* v)
{
	// cell offset 
	std::streampos offset = m_table_start + row_index * m_row_size;
	for (const Column& c : m_columns)
	{
		if (c == column)
			break;
		offset += c.get_size();
	}

	switch (column.get_type()) {
		case INTEGER: {
			int i = static_cast<IntegerValue*>(v)->int_val;
			table_file.write_at(offset, &i, TYPE_SIZE.at(INTEGER));
			break;
		}

		case REAL: {
			double d = static_cast<RealValue*>(v)->real_val;
			table_file.write_at(offset, &d, TYPE_SIZE.at(REAL));
			break;
		}

		case STRING: {
			std::array<char, STRING_SIZE> arr;
			table_file.write_at(offset, arr.data(), TYPE_SIZE.at(STRING));
			break;
		}
	};
}
			

/* repr */
std::ostream& operator<<(std::ostream& out, const Table& table)
{
	for (const Column& c : table.m_columns)
	{
		out << table.get_name() << " [" << c << "] ";
	};
	return out;
}

/* get column */
const Column& Table::get_column(const std::string& name) const
{
	auto findres {std::find_if(columns.begin(), columns.end(), [name](const Column& c){ return c.get_name() == name;})};
	if (findres == columns.end())
		throw no_such_column("no such column as '" + name + "'");

	return *findres;
}

std::string Table::get_file_data()
{
	table_file.seekg(0);
	return (std::stringstream() << table_file.rdbuf()).str();
}
