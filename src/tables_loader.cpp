#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
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

const Table& TablesLoader::get_table(const std::string& name) const
{
	auto find_result = std::find_if(tables.begin(), tables.end(), [name](std::unique_ptr<Table> ptr){ return ptr->get_name() == name; });

	if (find_result == tables.end())
		throw no_such_table("no such table as '" + name + "'...");

	return **find_result;
}
