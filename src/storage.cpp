
#include "storage.h"


bool write_at_pos(std::fstream& f, size_t position, const char* data, size_t size)
{
	if (!f.is_open())
		return false;

	if (f.seekp(position, std::ios::beg).fail())
		return false;

	return !f.write(data, size).fail();
}

bool read_at_pos(std::fstream& f, size_t position, char* data, size_t size)
{
	if (!f.is_open())
		return false;

	if (f.seekg(position, std::ios::beg).fail())
		return false;

	return !f.read(data, size).fail();
}
