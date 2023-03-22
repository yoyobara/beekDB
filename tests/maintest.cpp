#include "table/types.h"
#include <gtest/gtest.h>
#include <memory>

#define private public
#include "tables_loader.h"

int main()
{
	Table t("tables/mama_table");
	std::cout << t.rows_count << std::endl;
	std::cout << t.row_size << std::endl;

	std::cout << static_cast<IntegerValue*>(t.get_cell(0, t.columns.at(1)).get())->int_val << std::endl;

	return 0;
}
