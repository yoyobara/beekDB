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
		Column(const std::string& name, ColumnType type) : m_name(name), m_type(type) {};

		/* simple getters */
		inline const std::string& get_name() const { return m_name; }

		inline ColumnType get_type() const { return m_type; }

		inline int get_size() const { return table_storage::TYPE_SIZE.at(get_type()); }

		/* column comparison */
		inline bool operator==(const Column& other) const { return other.m_name == m_name; };

	private:
		std::string m_name;
		ColumnType m_type;
};

std::ostream& operator<<(std::ostream& out, const Column& c);

struct no_such_column : std::runtime_error
{
	no_such_column(const std::string& msg) : std::runtime_error(msg){}
};

struct Table;

/* represents a record in the table */
struct Record
{
	private:
		Table& of_table;
		std::unique_ptr<char> raw_data;

		template<typename ValueType>
		ValueType get(int offset);

	public:
		Record(Table& of_table, size_t file_pos);

		template<typename ValueType>
		ValueType get(Column& column);
};

struct Table
{
	friend class Record;
	
	Table(const fs::path& path);

	private:
		RandomAccessFile file;
		std::vector<Column> columns;

		std::string name;

		int record_size;
		long records_count;
};

void create_table(const fs::path& path);
