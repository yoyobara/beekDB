#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "storage.h"

using std::ios;

void RandomAccessFileHandler::write(size_t position, const char* data, size_t size) 
{
	m_file.seekp(position, std::ios::beg);
	m_file.write(data, size);
}

void RandomAccessFileHandler::read(size_t position, char* data, size_t size)
{
	m_file.seekg(position, std::ios::beg);
	m_file.read(data, size);
}

RandomAccessFileHandler::RandomAccessFileHandler(const std::string_view filename, bool create)
{
	if (create)
		// just create the file
		std::ofstream f(filename.data());
	
	m_file.exceptions(std::fstream::failbit); // set that an exception shall be thrown on io error
	m_file.open(filename.data(), ios::out | ios::in | ios::binary);
}
