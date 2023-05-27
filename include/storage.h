#pragma once

#include <fstream>
#include <mutex>
#include <filesystem>

namespace fs = std::filesystem;

/*
 * a class which represents a file that can be accessed at a specific position.
 */
class RandomAccessFile : public std::fstream
{
    // used to make operations thread-safe
    std::mutex m_mutex;

	public:
		/*
		 * constructs a new RandomAccessFile.
		 *
		 * @param filename - the path of the file.
		 * @param create - create a new file or not.
		 */
		RandomAccessFile(const fs::path& filename, bool create);

		/**
		 * read a file at a specific position.
		 *
		 * after the call the cursor is after the content read.
		 *
		 * @param position - the position to start reading 
		 * @param data - the buffer to read into
		 * @param size - how much to read.
		 */
		void read_at(size_t position, void* data, size_t size);

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
		void write_at(size_t position, const void* data, size_t size);

		/*
		 * verify that the content of the file starting at position,
		 * is equal to the given string.
		 */
		bool verify_content(size_t position, const std::string& s);
};
