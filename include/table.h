#pragma once

#include "storage.h"
#include <vector>
#include <string_view>

/* the three possible column types */
enum ColumnType
{
	INTEGER,
	REAL,
	VARCHAR
};

/*
 * represents a column name, type, properties
 */
class Column
{
	public:

		/*
		 * create a new column.
		 * 
		 * @param name - the column name
		 * @param type - the column type
		 * @param parameter - the column type parameter (for example length for varchar)
		 */
		Column(const std::string_view name, ColumnType type, int parameter);

		/* simple getters */
		std::string_view get_name() const;
		ColumnType get_type() const;
		int get_parameter() const;

	private:
		const std::string_view m_name;
		const ColumnType m_type;
		const int m_parameter;
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
		Table(const std::string_view name, const std::vector<Column>& columns);

		/*
		 * opens an existing table file.
		 * name - the name of the table.
		 */
		Table(const std::string_view name);

	private:
		
		const std::string_view name;
		std::vector<Column> columns;
};
