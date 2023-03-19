#pragma once
#include <vector>
#include <mutex>

#include "storage.h"
#include "table_storage_constants.h"

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
		int get_size() const;

	private:
		const std::string m_name;
		const ColumnType m_type;
		const int m_size;
};
std::ostream& operator<<(std::ostream& out, const Column& c);

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

		/*
		 * textual representation
		 */
		friend std::ostream& operator<<(std::ostream& out, const Table& table);

		/*
		 * access a cell in the table
		 */
		// TODO

	private:

		/* open */
		void init_metadata();
		void init_columns();

		/* create */
		void create_metadata(const std::vector<Column>& columns);

		// initializes the size of a whole row
		void init_row_size();

		RandomAccessFile table_file;

		// table's name
		const std::string name;

		// columns of the table
		std::vector<Column> columns;

		rows_count_t rows_count;
		columns_count_t columns_count;

		/* total size in bytes of a row */
		int row_size;

		// a mutex allowing only one to access the table at a time
		std::mutex mtx;
};
