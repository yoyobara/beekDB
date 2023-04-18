#include <algorithm>
#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <ios> 
#include <memory> 
#include <numeric>
#include <spdlog/spdlog.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "storage.h"
#include "table/table.h"
#include "table/table_storage_constants.h"
#include "table/types.h"
#include "utils.h"

using namespace table_storage;

/* column */

std::ostream& operator<<(std::ostream& out, const Column& c)
{
	out << c.get_name() << ": " << c.get_type();
	return out;
}

/* record */

Record::Record(const Table* of_table, size_t file_pos) : 
	of_table(of_table), 
	raw_data(new char[of_table->m_record_size])
{
	of_table->m_file.read_at(file_pos, raw_data.get(), of_table->m_record_size);
}

template<typename ValueType>
ValueType Record::get(int offset) const
{
	return ValueType(raw_data.get() + offset);
}

template<typename ValueType>
ValueType Record::get(Column& column) const
{
	int offset{0};
	for (const Column& c : of_table->m_columns)
	{
		if (c == column)
			break;
		offset += c.get_size();
	}

	return get<ValueType>(offset);
}

template IntegerValue Record::get<>(Column& column) const;
template RealValue Record::get<>(Column& column) const;
template VarChar50Value Record::get<>(Column& column) const;

/* table */

Table::Table(const fs::path& path) : 
	m_name(path),
	m_file(path, false)
{
	// verify signature
	if (!m_file.verify_content(SIGNATURE_OFFSET, SIGNATURE))
		spdlog::critical("table file is corrupted: {}", m_name);

	// read columns count
	int columns_count;
	m_file.read_at(COLUMN_COUNT_OFFSET, &columns_count, sizeof columns_count);

	// read records count
	m_file.read_at(RECORDS_COUNT_OFFSET, &m_records_count, sizeof m_records_count);
	
	// read columns (file pos now at first col)
	for (int i = 0 ; i < columns_count ; i++)
	{
		char descriptor;
		m_file.read(&descriptor, 1);

		char buff;
		std::string column_name;
		do
		{
			m_file.read(&buff, 1);
			column_name += buff;
		} while (buff != '\0');
		column_name.pop_back(); // '\0'
		
		m_columns.push_back(Column(column_name, BYTE_TO_TYPE.at(descriptor)));
	}
	// now pos at start of table data
}

void create_table(const fs::path& path, std::vector<Column> columns)
{
	// create a random access file
	RandomAccessFile f(path, true);

	std::stringstream strm("", std::ios::app | std::ios::out);

	// write metadata
	strm << SIGNATURE;

	// columns count
	int columns_count = columns.size();
	strm.write(reinterpret_cast<char*>(&columns_count), sizeof columns_count);

	// records count
	long records_count = 0;
	strm.write(reinterpret_cast<char*>(&records_count), sizeof records_count);

	for (Column& c : columns)
		strm << TYPE_TO_BYTE.at(c.get_type()) << c.get_name() << '\0';
	
	f << strm.rdbuf();
}

const Column& Table::get_column(const std::string& name) const
{
	auto findres = std::find_if(m_columns.begin(), m_columns.end(), [name](const Column& c)
	{
		return c.get_name() == name;
	});

	if (findres == m_columns.end())
		throw no_such_column("no such column as " + name);

	return *findres;
}


RecordIterator Table::begin() const
{
	return RecordIterator(this);
}
RecordIterator Table::end() const
{
	RecordIterator r(this);
	r.remaining_records = 0;

	return r;
}

