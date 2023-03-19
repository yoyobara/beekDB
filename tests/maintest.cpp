#include <gtest/gtest.h>
#include <memory>
#include "table.h"
#include "tables_loader.h"

int main()
{
	TablesLoader::get_instance();
	return 0;
}
