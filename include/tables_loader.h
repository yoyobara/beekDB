#pragma once

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include "table/table.h"


/*
 * the singleton in charge of loading the tables handlers.
 */
class TablesLoader
{
	private:

		TablesLoader();

		// the tables loaded.
		std::vector<std::unique_ptr<Table>> tables;

	public:

		// get a table by name
		Table& get_table(const std::string& name) const;

		// load tables from directory
		void reload_tables();

		// singleton essentials
		static TablesLoader& get_instance()
		{
			static TablesLoader instance;
            std::cout << "HERE\n";

			return instance;
		}

		TablesLoader(const TablesLoader& o) = delete;
		void operator=(const TablesLoader& o) = delete;

};
