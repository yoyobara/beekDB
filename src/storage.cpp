#include <algorithm>
#include <array>
#include <fstream>
#include <ios>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>

#include "storage.h"
#include "table/table_storage_constants.h"

using std::ios;

void RandomAccessFile::write_at(size_t position, const void* data, size_t size) 
{
    std::lock_guard<std::mutex> guard(m_mutex); // accuqire lock

	seekp(position, std::ios::beg);
	std::fstream::write(static_cast<const char*>(data), size);

    // lock released
}

void RandomAccessFile::read_at(size_t position, void* data, size_t size)
{
    std::lock_guard<std::mutex> guard(m_mutex); // accuqire lock
	seekg(position, std::ios::beg);
	read(static_cast<char*>(data), size);

    // lock released
}

RandomAccessFile::RandomAccessFile(const fs::path& filename, bool create)
{
	if (create)
		// just create the file
		std::ofstream f(filename);
	
	exceptions(std::fstream::failbit); // set that an exception shall be thrown on io error
	open(filename, ios::out | ios::in | ios::binary);
}

bool RandomAccessFile::verify_content(size_t position, const std::string& s)
{
    std::lock_guard<std::mutex> guard(m_mutex); // accuqire lock
 
    std::array<char, 6> buff;
	read_at(position, buff.data(), buff.size());

	return std::equal(buff.begin(), buff.end(), table_storage::SIGNATURE.begin());
    // lock released
}
