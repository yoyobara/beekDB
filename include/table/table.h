#pragma once

#include <cstdint>
#include <ios>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <vector>
#include <mutex>

#include "cpp_socket.h"
#include "storage.h"
#include "table/types.h"
#include "table_storage_constants.h"
#include "utils.h"

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
		inline const std::string& get_name() const { return m_name; }

		inline ColumnType get_type() const { return m_type; }

		inline int get_size() const { return table_storage::TYPE_SIZE.at(get_type()); }

		/* column comparison */
		inline bool operator==(const Column& other) const { return other.m_name == m_name; };

	private:
		const std::string m_name;
		const ColumnType m_type;
};

std::ostream& operator<<(std::ostream& out, const Column& c);

struct no_such_column : std::runtime_error
{
	no_such_column(const std::string& msg) : std::runtime_error(msg){}
};

/*
 * a class representing a table.
 */
class Table
{
	public:

		/*
		 * opens an existing table file.
		 * name - the name of the table.
		 */
		Table(const std::string& name);

		/*
		 * get unique ptr to heap allocated cell value
		 */
		std::unique_ptr<TableValue> get_cell(long row_index, const Column& column) const;

		/* get file data */
		std::string get_file_data();

		inline const std::string& get_name() const { return m_name; }

		/* get rows count */
		inline uint64_t get_rows_count() const{ return m_rows_count; }

		/* set cell value */
		void set_cell(long row_index, const Column& column, TableValue *v);

		/* get column by name */
		const Column& get_column(const std::string& name) const;

		/*
		 * textual representation
		 */
		friend std::ostream& operator<<(std::ostream& out, const Table& table);

	private:

		/* open */
		void init_metadata();
		void init_columns(int columns_count);

		// initializes the size of a whole row
		void init_row_size();

		uint64_t calculate_offset(long row_index, const Column& column) const;

		mutable RandomAccessFile m_table_file;

		// table's name
		const std::string m_name;

		// columns of the table
		std::vector<Column> m_columns;

		long m_rows_count;

		/* total size in bytes of a row */
		int m_row_size;

		/* start position of the table after the metadata */
		uint64_t m_table_start;
};

void create_table(const std::vector<Column> columns, const fs::path& name);
