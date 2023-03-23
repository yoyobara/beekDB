#include "table/types.h"
#include <gtest/gtest.h>
#include <memory>

#define private public
#include "tables_loader.h"

int main()
{
	Table t("tables/mama_table");

	std::cout << static_cast<IntegerValue*>(t.get_cell(0, t.get_column("age")).get())->int_val << std::endl;

	IntegerValue v(55);
	t.set_cell(0, t.get_column("age"), &v);

	std::cout << static_cast<IntegerValue*>(t.get_cell(0, t.get_column("age")).get())->int_val << std::endl;

	return 0;
}
