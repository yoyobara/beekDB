#pragma once

#include <string>

namespace table_storage 
{
	const int STRING_SIZE {50};

	const std::string SIGNATURE{"TABDEF"};
	const int SIGNATURE_OFFSET{0};

	const int COLUMN_COUNT_OFFSET{6};
	const int COLUMN_COUNT_SIZE{4};

	const int ROW_COUNT_OFFSET{10};
	const int ROW_COUNT_SIZE{8};
}
