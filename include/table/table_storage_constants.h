#pragma once

#include <filesystem>
#include <map>

#include "table/types.h"

namespace fs = std::filesystem;

namespace table_storage 
{
	// type sizes
	const std::map<char, ColumnType> BYTE_TO_TYPE {
		{'i', INTEGER},
		{'r', REAL},
		{'s', VARCHAR_50}
	};

	const std::map<ColumnType, char> TYPE_TO_BYTE {
		{INTEGER, 'i'},
		{REAL, 'r'},
		{VARCHAR_50, 's'}
	};

	const int VARCHAR_50_SIZE {50}; // need to be declared seperately since appearently we need it as a compile time constant.

	const std::map<ColumnType, int> TYPE_SIZE {
		{INTEGER, 4},
		{REAL, 8},
		{VARCHAR_50, VARCHAR_50_SIZE}
	};

	const std::string SIGNATURE{"TABDEF"};
	const int SIGNATURE_OFFSET{0};

	const int COLUMN_COUNT_OFFSET{6};

	const int RECORDS_COUNT_OFFSET{10};

	const int DESC_SIZE{1};

	const fs::path TABLES_DIR { fs::path("tables") };
	const fs::path TEMP_DIR { fs::path("tmp") };
}
