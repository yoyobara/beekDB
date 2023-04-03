#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <ios>
#include <memory>
#include <numeric>
#include <spdlog/spdlog.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "storage.h"
#include "table/table.h"
#include "table/table_storage_constants.h"
#include "table/types.h"
#include "utils.h"

using namespace table_storage;

/* column */

Column::Column(const std::string& name, ColumnType type) : m_name(name), m_type(type) {}

std::ostream& operator<<(std::ostream& out, const Column& c)
{
	out << c.get_name() << ": " << c.get_type();
	return out;
}


/* table */

/* open */
void Table::init_columns(int columns_count)
{
	// now cursor at start of columns.

	char buffer;

	for (int i = 0 ; i < columns_count ; i++)
	{
		// read descriptor
		m_table_file.read(&buffer, DESC_SIZE);

		ColumnType type {BYTE_TO_TYPE.at(buffer)};

		// read name
		std::string col_name;
		do {
			m_table_file.read(&buffer, 1);
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
	assert(m_table_file.verify_content(SIGNATURE_OFFSET, table_storage::SIGNATURE));

	int columns_count;

	// read columns count from metadata
	m_table_file.read_at(COLUMN_COUNT_OFFSET, &columns_count, sizeof columns_count);

	// read rows count from metadata.
	m_table_file.read_at(ROW_COUNT_OFFSET, &m_rows_count, sizeof m_rows_count);

	init_columns(columns_count);

	// now metadata is over, at start of table itself
	m_table_start = m_table_file.tellg();
}

/* open */
Table::Table(const std::string& name) :
	m_table_file(TABLES_DIR / name, false),
	m_name(name)
{
	init_metadata();
	init_row_size();
}

void Table::init_row_size()
{
	m_row_size = std::accumulate(m_columns.begin(), m_columns.end(), 0, [](int current, const Column& next){ return current + next.get_size();});
}

void create_table(const std::vector<Column> columns, const std::string& name)
{
	RandomAccessFile f(TABLES_DIR / name, true);

	// create the metadata

	f.write_at(SIGNATURE_OFFSET, SIGNATURE.data(), SIGNATURE.size()); // signature

	int columns_count = columns.size();
	f.write_at(COLUMN_COUNT_OFFSET, &columns_count, sizeof(int));

	long rows_count = 0;
	f.write_at(ROW_COUNT_OFFSET, &rows_count, sizeof(long));

	// now at columns offset
	for (const Column& col : columns)
	{
		f.write(&TYPE_TO_BYTE.at(col.get_type()), DESC_SIZE); // desc
		f.write(col.get_name().data(), col.get_name().size()); // name
		f.write("\0", 1); // null character
	}
}

/* calculate the offset of a cell in the file */
uint64_t Table::calculate_offset(long row_index, const Column& column) const
{
	uint64_t offset = m_table_start + row_index * m_row_size;

	for (const Column& c : m_columns)
	{
		if (c == column)
			break;

		offset += c.get_size();
	}

	return offset;
}

/* get cell */
std::unique_ptr<TableValue> Table::get_cell(long row_index, const Column& column) const
{
	// cell offset
	uint64_t offset {calculate_offset(row_index, column)};

	switch (column.get_type()) {
		case INTEGER:
			int i;
			m_table_file.read_at(offset, &i, TYPE_SIZE.at(INTEGER));

			return std::make_unique<IntegerValue>(i);

		case REAL:
			double d;
			m_table_file.read_at(offset, &d, TYPE_SIZE.at(REAL));

			return std::make_unique<RealValue>(d);

		case STRING:
			std::array<char, STRING_SIZE> buff;
			m_table_file.read_at(offset, buff.data(), TYPE_SIZE.at(STRING));

			return std::make_unique<StringValue>(buff.data());
	}
	spdlog::error("undefined type");
	return nullptr;
}


void Table::set_cell(long row_index, const Column& column, TableValue* v)
{
	spdlog::info("setting cell (row: {}, column: {})", row_index, column.get_name());
	
	// if in higher row index than currently has, needs to resize
	if (m_rows_count <= row_index) m_rows_count = row_index + 1;

	// cell offset 
	uint64_t offset { calculate_offset(row_index, column) };

	switch (column.get_type()) {
		case INTEGER: {
			int i = static_cast<IntegerValue*>(v)->int_val;
			m_table_file.write_at(offset, &i, TYPE_SIZE.at(INTEGER));
			break;
		}

		case REAL: {
			double d = static_cast<RealValue*>(v)->real_val;
			m_table_file.write_at(offset, &d, TYPE_SIZE.at(REAL));
			break;
		}

		case STRING: {
			std::array<char, STRING_SIZE> arr;
			m_table_file.write_at(offset, arr.data(), TYPE_SIZE.at(STRING));
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
	auto findres {std::find_if(m_columns.begin(), m_columns.end(), [name](const Column& c){ return c.get_name() == name;})};
	if (findres == m_columns.end())
		throw no_such_column("no such column as '" + name + "'");

	return *findres;
}

std::string Table::get_file_data()
{
	m_table_file.seekg(0);
	return (std::stringstream() << m_table_file.rdbuf()).str();
}
