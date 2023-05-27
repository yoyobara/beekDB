#pragma once

#include <algorithm>
#include <functional>
#include <cassert>
#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <vector>
#include <mutex>

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

struct Table;

/* represents a record in the table 
 *
 * when a record is initialized, it hold the data for the whole record on the heap
 * using a smart pointer, and provides interface to get stuff from the record, and put 
 * new stuff (in the table file also)
 *
 * */
struct Record
{
	public:
		const Table *of_table;
		std::unique_ptr<char> raw_data;
		long data_pos;

		template<typename ValueType>
		ValueType get(int offset) const;


	public: 
		Record(const Table* of_table, size_t file_pos); // link record with its table 
		Record(const Table* of_table); // empty record probably for later insertion

		/*
		 * get a value of a column in the record.
		 */
		template<typename ValueType>
		ValueType get(const std::string& column_name) const;

		/* 
		 * put a new value on the record temporary buffer
		 */
		template<typename ValueType>
		void put(const std::string& column_name, ValueType value);

		/*
		 * updates the table according to the record's temporary buffer
		 */
		void update() const;
};

struct Table
{
	Table(const fs::path& path);

	const std::string& get_name() const { return m_name; }
	const int get_records_count() const { return m_records_count; }
	const size_t get_new_record_offset() const {return m_data_offset + m_record_size * m_records_count; }

	// get an unmodifiable vector of columns
	const std::vector<Column>& get_columns() const { return m_columns; }

	// get a single column by name
	const Column& get_column(const std::string& name) const;

	/*
	 * insert a new record into table with values.
	 * a values which is nullptr is leaving the cell uninitialized.
	 */
	void insert(const Record& rec);

	std::string get_file_data();

	/* does specific stuff on each record of this table */
	void for_each(std::function<void(Record&&)> func) const;

    // throws an `corrupted_table` exception if the table is corrupted
    void verify_not_corrupted();

	friend class Record;

	private:
		// file that the table manages
		mutable RandomAccessFile m_file;
		
		// column vector
		std::vector<Column> m_columns;

		// column name
		std::string m_name;

		// size in bytes of a single record in the table
		int m_record_size;

		long m_records_count;

		// the offset in bytes of where the data starts (see table storage protocol)
		long m_data_offset;

		size_t get_column_offset(const Column& c) const;
		size_t get_column_offset(const std::string& column_name) const;
};

void create_table(const fs::path& path, std::vector<Column> columns);
