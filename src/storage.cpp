#include <ios>
#include <iostream>
#include <string>

#include "storage.h"

using std::ios;

bool PositionalFile::write_at_pos(size_t position, const char* data, size_t size)
{
	if (!file.is_open())
		return false;

	if (file.seekp(position, std::ios::beg).fail())
		return false;

	return !file.write(data, size).fail();
}

bool PositionalFile::read_at_pos(size_t position, char* data, size_t size)
{
	if (!file.is_open())
		return false;

	if (file.seekg(position, std::ios::beg).fail())
		return false;

	return !file.read(data, size).fail();
}


PositionalFile::PositionalFile(const std::string& filename) : file(filename, ios::out | ios::in)
{}
