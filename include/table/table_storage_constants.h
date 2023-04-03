#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <filesystem>

#include "table/types.h"

namespace fs = std::filesystem;

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

	const int STRING_SIZE {50};

	const std::map<ColumnType, int> TYPE_SIZE {
		{INTEGER, 4},
		{REAL, 8},
		{STRING, STRING_SIZE}
	};

	const std::string SIGNATURE{"TABDEF"};
	const int SIGNATURE_OFFSET{0};

	const int COLUMN_COUNT_OFFSET{6};

	const int ROW_COUNT_OFFSET{10};

	const int DESC_SIZE{1};

	const fs::path TABLES_DIR { fs::path("tables") };
	const fs::path TEMP_DIR { fs::path("tmp") };
}
