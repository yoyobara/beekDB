#pragma once

#include <string>

using columns_count_t = int;
using rows_count_t = long;

namespace table_storage 
{
	const int STRING_SIZE {50};

	const std::string SIGNATURE{"TABDEF"};
	const int SIGNATURE_OFFSET{0};

	const int COLUMN_COUNT_OFFSET{6};

	const int ROW_COUNT_OFFSET{10};

	const int DESC_SIZE{1};
}
