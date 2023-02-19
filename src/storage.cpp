#include "storage.h"
#include <ios>
#include <string>

bool TableFile::write_at_pos(size_t position, const char* data, size_t size)
{
	if (!file.is_open())
		return false;

	if (file.seekp(position, std::ios::beg).fail())
		return false;

	return !file.write(data, size).fail();
}

bool TableFile::read_at_pos(size_t position, char* data, size_t size)
{
	if (!file.is_open())
		return false;

	if (file.seekg(position, std::ios::beg).fail())
		return false;

	return !file.read(data, size).fail();
}


TableFile::TableFile(const std::string& filename) : file(filename, std::ios::out | std::ios::in)
{}
