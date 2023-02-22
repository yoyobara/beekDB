#include "storage.h"
#include <table.h>
#include <vector>

/* column */

Column::Column(const std::string_view name, ColumnType type, int parameter) : 
	m_name(name), m_type(type), m_parameter(parameter) {}

std::string_view Column::get_name() const { return m_name; }
ColumnType Column::get_type() const { return m_type; }
int Column::get_parameter() const { return m_parameter; }


/* table */

Table::Table(const std::string_view name) : RandomAccessFileHandler(name, false)
{

}

Table::Table(const std::string_view name, const std::vector<Column>& columns) : RandomAccessFileHandler(name, true)
{

}
