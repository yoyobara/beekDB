#include "table/types.h"
#include <gtest/gtest.h>
#include <memory>

#define private public
#include "tables_loader.h"

int main()
{
	Table t("tables/mama_table");

	IntegerValue v(55);
	t.set_cell(0, t.get_column("age"), &v);

	return 0;
}
