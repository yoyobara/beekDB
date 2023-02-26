#include <cassert>
#include <vector>
#include <iostream>

#include "table.h"
#include "storage.h"
#include "table_storage_constants.h"

/* column */

Column::Column(const std::string_view name, ColumnType type) : 
	m_name(name), m_type(type) {}

std::string_view Column::get_name() const { return m_name; }
ColumnType Column::get_type() const { return m_type; }


/* table */

Table::Table(const std::string_view name) :
	table_file(name, false)
{
	// assert signature presence TODO
	assert(table_file.verify_content(table_storage::SIGNATURE_OFFSET, table_storage::SIGNATURE));

	// read columns count from metadata
	table_file.read_at(table_storage::COLUMN_COUNT_OFFSET, &columns_count, table_storage::COLUMN_COUNT_SIZE);

	// read rows count from metadata.
	table_file.read_at(table_storage::ROW_COUNT_OFFSET, &rows_count, table_storage::ROW_COUNT_SIZE);
}

Table::Table(const std::string_view name, const std::vector<Column>& columns) :
	table_file(name, true),
	columns(columns),
	name(name)
{
	
}
