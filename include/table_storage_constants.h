#pragma once

#include <map>
#include <string>

using columns_count_t = int;
using rows_count_t = long;


/* the three possible column types */
enum ColumnType
{
	INTEGER,
	REAL,
	STRING
};

namespace table_storage 
{
	// type sizes
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

	const std::map<ColumnType, int> TYPE_SIZE {
		{INTEGER, 4},
		{REAL, 8},
		{STRING, 50}
	};

	const std::string SIGNATURE{"TABDEF"};
	const int SIGNATURE_OFFSET{0};

	const int COLUMN_COUNT_OFFSET{6};

	const int ROW_COUNT_OFFSET{10};

	const int DESC_SIZE{1};
}
