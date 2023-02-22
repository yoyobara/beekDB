#pragma once

#include <cassert>
#include <fstream>
#include <string_view>

/*
 * a class which represents a handler of a 
 * file that can be accessed in a specific position.
 */
class RandomAccessFileHandler
{
	private:
		std::fstream m_file;

	public:
		/*
		 * constructs a new RandomAccessFileHandler.
		 *
		 * @param filename - the name of the file.
		 * @param create - create a new file or not.
		 */
		RandomAccessFileHandler(const std::string_view filename, bool create);

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
