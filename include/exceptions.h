#pragma once
#include "table/table.h"
#include <stdexcept>

/*
this file contains all the exceptions that the program might throw.
the program should not ACTUALLY throw anything, so they must be caught before they interrupt the 
program
*/

struct beek_exception : std::runtime_error
{
    beek_exception(const std::string& message) : std::runtime_error(message) {}
};

struct no_such_column : beek_exception
{
	no_such_column(const std::string& msg) : beek_exception(msg){}
};

struct corrupted_table : beek_exception
{
    corrupted_table(const std::string& msg) : beek_exception(msg) {}
};

struct incorrect_type : beek_exception
{
	incorrect_type(const std::string& msg) : beek_exception(msg){}
    static inline incorrect_type classic(const std::string& column_name)
    {
        return incorrect_type("invalid literal type for column " + column_name);
    }
};

struct not_implemented : beek_exception
{
    not_implemented(const std::string& msg) : beek_exception(msg){}
};

struct no_such_table : beek_exception
{
	no_such_table(const std::string& msg) : beek_exception(msg) {}
};

struct where_clause_error : beek_exception
{
    where_clause_error(const std::string& msg) : beek_exception(msg) {}
};

struct table_name_taken : beek_exception 
{
    table_name_taken(const std::string& msg) : beek_exception(msg){}
};
