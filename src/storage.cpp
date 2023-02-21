#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "storage.h"

using std::ios;

void PositionalFileHandler::write(size_t position, const char* data, size_t size) 
{
	file.seekp(position, std::ios::beg);
	file.write(data, size);
}

void PositionalFileHandler::read(size_t position, char* data, size_t size)
{
	file.seekg(position, std::ios::beg);
	file.read(data, size);
}

PositionalFileHandler::PositionalFileHandler(const std::string& filename)
{
	file.exceptions(std::fstream::failbit); // set that an exception shall be thrown on io error

	file.open(filename, ios::out | ios::in | ios::binary);
}

PositionalFileHandler PositionalFileHandler::open(const std::string &filename)
{
	return PositionalFileHandler(filename);
}

PositionalFileHandler PositionalFileHandler::create(const std::string &filename)
{
	// just create the file
	std::ofstream f(filename);

	return open(filename);
}
