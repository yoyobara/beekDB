#include <table.h>
#include <vector>
#include <iostream>

#include "storage.h"
#include "table_storage_constants.h"

/* column */

Column::Column(const std::string_view name, ColumnType type, int parameter) : 
	m_name(name), m_type(type), m_parameter(parameter) {}

std::string_view Column::get_name() const { return m_name; }
ColumnType Column::get_type() const { return m_type; }
int Column::get_parameter() const { return m_parameter; }


/* table */

Table::Table(const std::string_view name) : RandomAccessFileHandler(name, false)
{
	// read rows count from metadata. kinda dirty, but reading bytes into the long type.
	read(table_storage::ROW_COUNT_OFFSET, &rows_count, table_storage::ROW_COUNT_SIZE);
	std::cout << rows_count << std::endl; //NOTE
}

Table::Table(const std::string_view name, const std::vector<Column>& columns) : 
	RandomAccessFileHandler(name, true), 
	columns(columns),
	name(name)
{
	
}
