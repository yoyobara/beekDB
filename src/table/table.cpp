#include <algorithm>
#include <array>
#include <cstdint>
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
