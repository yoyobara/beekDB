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

Record::Record(Table& of_table, size_t file_pos) : 
	of_table(of_table), 
	raw_data(new char[of_table.record_size])
{
	of_table.file.read_at(file_pos, raw_data.get(), of_table.record_size);
}

template<typename ValueType>
ValueType Record::get(int offset)
{
	return ValueType(raw_data.get()[offset]);
}

template<typename ValueType>
ValueType Record::get(Column& column)
{
	int offset{0};
	for (Column& c : of_table.columns)
	{
		if (c == column)
			break;
		offset += c.get_size();
	}

	return get<ValueType>(offset);
}

/* table */

Table::Table(const fs::path& path) : 
	name(path),
	file(path, false)
{
	// verify signature
	if (!file.verify_content(SIGNATURE_OFFSET, SIGNATURE))
		spdlog::critical("table file is corrupted: {}", name);

	// read columns count
	int columns_count;
	file.read_at(COLUMN_COUNT_OFFSET, &columns_count, sizeof columns_count);

	// read records count
	file.read_at(RECORDS_COUNT_OFFSET, &records_count, sizeof records_count);
	
	// read columns (file pos now at first col)
	for (int i = 0 ; i < columns_count ; i++)
	{
		char descriptor;
		file.read(&descriptor, 1);

		char buff;
		std::string column_name;
		do
		{
			file.read(&buff, 1);
			column_name += buff;
		} while (buff != '\0');
		column_name.pop_back(); // '\0'
		
		columns.push_back(Column(column_name, BYTE_TO_TYPE.at(descriptor)));
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
}
