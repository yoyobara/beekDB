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

Table::Table(const std::string_view name) : RandomAccessFileHandler(name, false)
{
	// assert signature presence TODO

	// read columns count from metadata
	read(table_storage::COLUMN_COUNT_OFFSET, &columns_count, table_storage::COLUMN_COUNT_SIZE);

	// read rows count from metadata.
	read(table_storage::ROW_COUNT_OFFSET, &rows_count, table_storage::ROW_COUNT_SIZE);

	const void* columns_start;

	for (int i = 0 ; i < columns_count ; i++)
	{

		
	}
}

Table::Table(const std::string_view name, const std::vector<Column>& columns) : 
	RandomAccessFileHandler(name, true), 
	columns(columns),
	name(name)
{
	
}
