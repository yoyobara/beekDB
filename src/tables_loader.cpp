#include <filesystem>
#include <iostream>
#include <memory>
#include "table.h"
#include "tables_loader.h"

namespace fs = std::filesystem;
using std::make_unique;

TablesLoader::TablesLoader()
{
	for (const auto& table_file : fs::directory_iterator("tables"))
	{
		// add unique ptr to table
		tables.push_back(make_unique<Table>(table_file.path()));
	}

	for (const std::unique_ptr<Table>& table : tables)
	{
		std::cout << *table << std::endl;
	}
}
