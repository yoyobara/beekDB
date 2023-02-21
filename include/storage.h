#pragma once

#include <cassert>
#include <fstream>
#include <string_view>

/*
 * a class which represents a handler of a 
 * file that can be accessed in a specific position.
 */
class PositionalFileHandler
{
	private:
		std::fstream m_file;

		/*
		 * filename must be a file that already exists
		 */
		PositionalFileHandler(const std::string_view filename);

	public:

		/* open an existing file */
		static PositionalFileHandler open(const std::string_view filename);

		/* create a new file */
		static PositionalFileHandler create(const std::string_view filename);

		/**
		 * read a file at a specific position.
		 *
		 * after the call the cursor is in undefined position.
		 *
		 * @param position - the position to start reading 
		 * @param data - the buffer to read into
		 * @param size - how much to read.
		 */
		void read(size_t position, char* data, size_t size);

		/**
		 * write a file at a specific position.
		 * override the file content at that position.
		 *
		 * after the call the cursor is in undefined position.
		 *
		 * @param position - the position to start writing
		 * @param data - the buffer to write into
		 * @param size - how much to write.
		 */
		void write(size_t position, const char* data, size_t size);
};
