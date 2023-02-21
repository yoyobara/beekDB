#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "storage.h"

using std::ios;

void PositionalFileHandler::write(size_t position, const char* data, size_t size) 
{
	m_file.seekp(position, std::ios::beg);
	m_file.write(data, size);
}

void PositionalFileHandler::read(size_t position, char* data, size_t size)
{
	m_file.seekg(position, std::ios::beg);
	m_file.read(data, size);
}

PositionalFileHandler::PositionalFileHandler(const std::string_view filename)
{
	m_file.exceptions(std::fstream::failbit); // set that an exception shall be thrown on io error

	m_file.open(filename.data(), ios::out | ios::in | ios::binary);
}

PositionalFileHandler PositionalFileHandler::open(const std::string_view filename)
{
	return PositionalFileHandler(filename);
}

PositionalFileHandler PositionalFileHandler::create(const std::string_view filename)
{
	// just create the file
	std::ofstream f(filename.data());

	return open(filename);
}
