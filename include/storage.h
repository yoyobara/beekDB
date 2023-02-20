#pragma once

#include <cassert>
#include <fstream>
#include <string>

/*
 * a class which represents a file to be read and written at a specific position.
 */
class PositionalFile
{
	private:
		std::fstream file;

	public:
		PositionalFile(const std::string& filename);

		/**
		 * read a file at a specific position.
		 *
		 * after the call the cursor is in undefined position.
		 */
		bool read_at_pos(size_t position, char* data, size_t size);

		/**
		 * write a file at a specific position.
		 * override the file content at that position.
		 *
		 * after the call the cursor is in undefined position.
		 */
		bool write_at_pos(size_t position, const char* data, size_t size);
};

/*
 * a class representing a table file
 */
class TableFile : public PositionalFile
{
};
