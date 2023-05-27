#pragma once
#include <stdexcept>

/*
this file contains all the exceptions that the program might throw.
the program should not ACTUALLY throw anything, so they must be caught before they interrupt the 
program
*/

struct BeekException : std::runtime_error
{
    BeekException(const std::string& message) : std::runtime_error(message) {}
};

struct no_such_column : BeekException
{
	no_such_column(const std::string& msg) : BeekException(msg){}
};

struct corrupted_table : BeekException
{
    corrupted_table(const std::string& msg) : BeekException(msg) {}
};

struct incorrect_type_exception : BeekException
{
	incorrect_type_exception(const std::string& msg) : BeekException(msg){}
};

struct not_implemented_exception : BeekException
{
    not_implemented_exception(const std::string& msg) : BeekException(msg){}
};
