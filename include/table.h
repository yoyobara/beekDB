#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <map>

#include "storage.h"

/* the three possible column types */
enum ColumnType
{
	INTEGER,
	REAL,
	STRING
};

const std::map<char, ColumnType> BYTE_TO_TYPE {
	{'i', INTEGER},
	{'r', REAL},
	{'s', STRING}
};

const std::map<ColumnType, char> TYPE_TO_BYTE {
	{INTEGER, 'i'},
	{REAL, 'r'},
	{STRING, 's'}
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
		 */
		Column(const std::string& name, ColumnType type);

		/* simple getters */
		std::string_view get_name() const;
		ColumnType get_type() const;

	private:
		const std::string m_name;
		const ColumnType m_type;
};

const std::string TABLE_FILE_START_PHRASE {"TABDEF"};

/*
 * a class representing a table.
 * subclasses PositionalFileHandler so the table can handle its file.
 */
class Table
{
	public:

		/*
		 * creates a new table file.
		 * @param name - the name of the table
		 * @param columns - vecotr of table columns
		 */
		Table(const std::string& name, const std::vector<Column>& columns);

		/*
		 * opens an existing table file.
		 * name - the name of the table.
		 */
		Table(const std::string& name);

	private:
		void init_metadata();
		void init_columns();
		
		RandomAccessFile table_file;
		const std::string name;

		std::vector<Column> columns;
		long rows_count;
		int columns_count;
};
