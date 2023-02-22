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

		/*
		 * create a new column.
		 * 
		 * @param name - the column name
		 * @param type - the column type
		 * @param parameter - the column type parameter (for example length for varchar)
		 */
		Column(std::string_view name, ColumnType type, int parameter);

	private:
		const std::string_view m_name;
		const ColumnType m_type;
		const int parameter;
};

const std::string TABLE_FILE_START_PHRASE {"TABDEF"};

/*
 * a class representing a table.
 * subclasses PositionalFileHandler so the table can handle its file.
 */
class Table : public RandomAccessFileHandler
{
	public:

		/*
		 * creates a new table file.
		 * name - the name of the table
		 */
		static Table create_table(const std::string_view name, std::vector<Column> columns);

		/* open existing table */
		static Table open_table(const std::string_view name);

	private:

		/*
		 * opens an existing table file.
		 * name - the name of the table.
		 */
		Table(const std::string_view name);
		
		const std::string_view name;
		std::vector<Column> columns;
};
