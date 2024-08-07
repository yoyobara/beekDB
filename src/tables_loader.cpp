#include <filesystem>

#include "tables_loader.h"
#include "exceptions.h"
#include <algorithm>

namespace fs = std::filesystem;

TablesLoader::TablesLoader() : tables()
{
	reload_tables();
}

void TablesLoader::reload_tables()
{
	for (const auto& table_file : fs::directory_iterator(table_storage::TABLES_DIR))
	{
		// if already there dont add it
		if (std::find_if(tables.begin(), tables.end(), [table_file](const std::unique_ptr<Table>& ptr){ return ptr->get_name() == table_file.path(); }) == tables.end())
		{
			// add unique ptr to table
			tables.emplace_back(std::make_unique<Table>(table_file.path()));
		}
	}
}

Table& TablesLoader::get_table(const std::string& name) const
{
	auto find_result = std::find_if(tables.begin(), tables.end(), [name](const std::unique_ptr<Table>& ptr){ return ptr->get_name() == (table_storage::TABLES_DIR / name); });

	if (find_result == tables.end())
		throw no_such_table("no such table as '" + name + "'...");

    Table& chosen_table = **find_result;

    chosen_table.verify_not_corrupted();

	return chosen_table;
}
