#pragma once

#include "storage.h"

/*
 * a class representing a table.
 * subclasses PositionalFileHandler so the table can handle its file.
 */
class Table : public PositionalFileHandler
{
	public:

		/*
		 * creates a new table file.
		 * name - the name of the table
		 */
		static Table create_table(const std::string& name);

		/* open existing table */
		static Table open_table(const std::string& name);
	
	private:

		/*
		 * opens an existing table file.
		 * name - the name of the table.
		 */
		Table(const std::string& name);
		
		/* table name */
		const std::string name;
};
