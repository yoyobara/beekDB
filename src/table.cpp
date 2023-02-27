#include <cassert>
#include <vector>
#include <iostream>

#include "table.h"
#include "storage.h"
#include "table_storage_constants.h"

/* column */

Column::Column(const std::string& name, ColumnType type) : 
	m_name(name), m_type(type) {}

std::string_view Column::get_name() const { return m_name; }
ColumnType Column::get_type() const { return m_type; }


/* table */

void Table::init_columns()
{
	// now cursor at start of columns.

	char buffer;

	for (int i = 0 ; i < columns_count ; i++)
	{
		// read descriptor
		table_file.read(&buffer, table_storage::DESC_SIZE);

		ColumnType type {BYTE_TO_TYPE.at(buffer)};

		// read name
		std::string name;
		do {
			table_file.read(&buffer, 1);
			name += buffer;
		}
		while (buffer != '\0');
		name.pop_back(); // \0

		// add column
		columns.push_back(Column(name, type));
	}
}

void Table::init_metadata()
{
	// assert signature presence
	assert(table_file.verify_content(table_storage::SIGNATURE_OFFSET, table_storage::SIGNATURE));

	// read columns count from metadata
	table_file.read_at(table_storage::COLUMN_COUNT_OFFSET, &columns_count, table_storage::COLUMN_COUNT_SIZE);

	// read rows count from metadata.
	table_file.read_at(table_storage::ROW_COUNT_OFFSET, &rows_count, table_storage::ROW_COUNT_SIZE);

	init_columns();
}

Table::Table(const std::string& name) :
	table_file(name, false)
{
}

Table::Table(const std::string& name, const std::vector<Column>& columns) :
	table_file(name, true),
	columns(columns),
	name(name)
{
	
}
