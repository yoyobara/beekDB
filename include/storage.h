#pragma once

#include <cassert>
#include <fstream>
#include <string_view>

#include "table_storage_constants.h"

/*
 * a class which represents a file that can be accessed at a specific position.
 */
class RandomAccessFile : private std::fstream
{
	public:
		/*
		 * constructs a new RandomAccessFile.
		 *
		 * @param filename - the name of the file.
		 * @param create - create a new file or not.
		 */
		RandomAccessFile(const std::string_view filename, bool create);

		/**
		 * read a file at a specific position.
		 *
		 * after the call the cursor is after the content read.
		 *
		 * @param position - the position to start reading 
		 * @param data - the buffer to read into
		 * @param size - how much to read.
		 */
		void read(size_t position, void* data, size_t size);

		/**
		 * write a file at a specific position.
		 * override the file content at that position.
		 *
		 * after the call the cursor is after the content written
		 *
		 * @param position - the position to start writing
		 * @param data - the buffer to write into
		 * @param size - how much to write.
		 */
		void write(size_t position, const void* data, size_t size);

		/*
		 * verify that the content of the file starting at position,
		 * is equal to the given string.
		 */
		bool verify_content(size_t position, std::string s);
};
