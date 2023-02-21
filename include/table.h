#pragma once

#include "storage.h"
#include <string_view>
#include <vector>

/*
 * represents a column name, type, properties
 */
class Column
{
	public:

		/* the three possible column types */
		enum ColumnType
		{
			INTEGER,
			REAL,
			VARCHAR
		};

		Column(std::string_view name);

	private:
		const bool m_primary_key;
		const std::string_view m_name;
		const ColumnType m_type;
};

/*
 * a class representing a table.
 * subclasses PositionalFileHandler so the table can handle its file.
 */
class Table : public PositionalFileHandler
{
	public:

		/*
		 * creates a new table file.
		 * name - the name of the table
		 */
		static Table create_table(const std::string_view name);

		/* open existing table */
		static Table open_table(const std::string_view name);
	
	private:

		/*
		 * opens an existing table file.
		 * name - the name of the table.
		 */
		Table(const std::string_view name);
		
		/* table name */
		const std::string_view name;

		std::vector<Column> columns;
};
