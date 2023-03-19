#include <numeric>
#include <sstream>
#include <iostream>

#include "table.h"
#include "utils.h"

/* column */

Column::Column(const std::string& name, ColumnType type) : 
	m_name(name), m_type(type), m_size(table_storage::TYPE_SIZE.at(type)) {}

std::string_view Column::get_name() const { return m_name; }
ColumnType Column::get_type() const { return m_type; }
int Column::get_size() const { return m_size; }

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

	for (int i = 0 ; i < columns_count ; i++)
	{
		// read descriptor
		table_file.read(&buffer, table_storage::DESC_SIZE);

		ColumnType type {table_storage::BYTE_TO_TYPE.at(buffer)};

		// read name
		std::string col_name;
		do {
			table_file.read(&buffer, 1);
			col_name += buffer;
		}
		while (buffer != '\0');
		col_name.pop_back(); // \0

		// add column
		columns.push_back(Column(col_name, type));
	}
}

void Table::init_metadata()
{
	// assert signature presence
	assert(table_file.verify_content(table_storage::SIGNATURE_OFFSET, table_storage::SIGNATURE));

	// read columns count from metadata
	table_file.read_at(table_storage::COLUMN_COUNT_OFFSET, &columns_count, sizeof columns_count);

	// read rows count from metadata.
	table_file.read_at(table_storage::ROW_COUNT_OFFSET, &rows_count, sizeof rows_count);

	init_columns();
}

/* open */
Table::Table(const std::string& name) :
	table_file(name, false),
	name(name)
{
	init_metadata();
	init_row_size();
}

void Table::init_row_size()
{
	row_size = std::accumulate(columns.begin(), columns.end(), 0, [](int current, const Column& next){ return current + next.get_size();});
}

/* create */

void Table::create_metadata(const std::vector<Column>& columns)
{
	// the file is currently open for writing at position 0
	std::stringstream ss;
	ss << table_storage::SIGNATURE;

	// columns count and rows count
	columns_count_t col_count = columns.size();
	ss << encode(col_count);
	ss << encode(static_cast<rows_count_t>(0));

	// columns themselves
	for (const Column& c : columns)
	{
		// descriptor
		ss << table_storage::TYPE_TO_BYTE.at(c.get_type());

		// name and \0
		ss << c.get_name() << '\0';
	}

	// finally write to file
	table_file << ss.rdbuf();
}

/* create */
Table::Table(const std::string& name, const std::vector<Column>& columns) :
	table_file(name, true),
	columns(columns),
	name(name)
{
	create_metadata(columns);
	init_row_size();
}

/* repr */
std::ostream& operator<<(std::ostream& out, const Table& table)
{
	for (const Column& c : table.columns)
	{
		out << '[' << c << "] ";
	};
	return out;
}
