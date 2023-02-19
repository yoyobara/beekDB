#pragma once

#include <cassert>
#include <fstream>

/**
 * write a file at a specific position.
 * override the file content at that position.
 *
 * after the call the cursor is in undefined position.
 *
 * file has to be opened with out | in
 */
bool write_at_pos(std::fstream& f, size_t position, const char* data, size_t size);

/**
 * read a file at a specific position.
 *
 * after the call the cursor is in undefined position.
 *
 * file has to be opened with out | in
 */
bool read_at_pos(std::fstream& f, size_t position, char* data, size_t size);
