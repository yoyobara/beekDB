#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include "tables_loader.h"

namespace fs = std::filesystem;

TablesLoader::TablesLoader()
{
	for (const auto& table_file : fs::directory_iterator(table_storage::TABLES_DIR))
	{
		// add unique ptr to table
		tables.emplace_back(std::make_unique<Table>(table_file.path()));
	}
}

const Table& TablesLoader::get_table(const std::string& name) const
{
	auto find_result = std::find_if(tables.begin(), tables.end(), [name](const std::unique_ptr<Table>& ptr){ return ptr->get_name() == name; });

	if (find_result == tables.end())
		throw no_such_table("no such table as '" + name + "'...");

	return **find_result;
}
