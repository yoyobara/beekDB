#pragma once

#include <algorithm>
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
		const Table *of_table;
		std::unique_ptr<char> raw_data;
		long data_pos;

		template<typename ValueType>
		ValueType get(int offset) const;

		friend class RecordIterator;

	public:
		Record(const Table* of_table, size_t file_pos);

		template<typename ValueType>
		ValueType get(const std::string& column_name) const;
};

struct RecordIterator;

struct Table
{
	Table(const fs::path& path);

	const std::string& get_name() const { return m_name; }
	const int get_records_count() const { return m_records_count; }

	const std::vector<Column>& get_columns() const { return m_columns; }
	const Column& get_column(const std::string& name) const;

	friend class Record;
	friend class RecordIterator;

	RecordIterator begin() const;
	RecordIterator end() const;

	private:
		mutable RandomAccessFile m_file;
		std::vector<Column> m_columns;

		std::string m_name;

		int m_record_size;
		long m_records_count;
		long m_data_offset;
};

/* class to iterate over records of a table */
struct RecordIterator
{
	RecordIterator(const Table* of_table) : 
		of_table(of_table),
		remaining_records(of_table->m_records_count),
		current_record(of_table, of_table->m_data_offset)
	{}

	inline const Record& operator*() { return current_record; }

	inline RecordIterator& operator++()
	{
		remaining_records--;
		current_record = Record(of_table, current_record.data_pos + of_table->m_record_size);
		return *this;
	}

	inline bool operator==(RecordIterator& other)
	{
		return of_table == other.of_table && remaining_records == other.remaining_records;
	}

	inline bool operator!=(RecordIterator& other)
	{
		return !(*this == other);
	}

	friend class Table;

	private:
		const Table *of_table;
		Record current_record;
		long remaining_records;
};

void create_table(const fs::path& path, std::vector<Column> columns);
