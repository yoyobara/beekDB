#include <array>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "storage.h"

using std::ios;

void RandomAccessFile::write_at(size_t position, const void* data, size_t size) 
{
	
	seekp(position, std::ios::beg);
	std::fstream::write(static_cast<const char*>(data), size);
}

void RandomAccessFile::read_at(size_t position, void* data, size_t size)
{
	seekg(position, std::ios::beg);
	read(static_cast<char*>(data), size);
}

RandomAccessFile::RandomAccessFile(const std::string_view filename, bool create)
{
	if (create)
		// just create the file
		std::ofstream f(filename.data());
	
	exceptions(std::fstream::failbit); // set that an exception shall be thrown on io error
	open(filename.data(), ios::out | ios::in | ios::binary);
}

bool RandomAccessFile::verify_content(size_t position, std::string s)
{
	std::string buff(s.size(), 0);
	read_at(position, buff.data(), s.size());

	return buff == s;
}
