#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "table/table.h"

struct no_such_table : std::runtime_error
{
	no_such_table(const std::string& msg) : std::runtime_error(msg){}
};

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
		const Table& get_table(const std::string& name) const;

		// singleton essentials
		inline static TablesLoader& get_instance()
		{
			static TablesLoader instance;

			return instance;
		}

		TablesLoader(const TablesLoader& o) = delete;
		void operator=(const TablesLoader& o) = delete;

};
