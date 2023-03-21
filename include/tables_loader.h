#pragma once

#include <memory>
#include <vector>
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

		// singleton essentials
		inline static TablesLoader& get_instance()
		{
			static TablesLoader instance;

			return instance;
		}

		TablesLoader(const TablesLoader& o) = delete;
		void operator=(const TablesLoader& o) = delete;

};
