#pragma once
#include <algorithm>
#include <array>
#include <string>

/* the three possible column types */
enum ColumnType
{
	INTEGER,
	REAL,
	STRING
};


/* an abstract table value. */
struct TableValue
{
	virtual ColumnType get_type() const = 0;
};

/* integer type value */
struct IntegerValue : TableValue
{

	int int_val;

	IntegerValue(int i) : int_val(i) {}

	ColumnType get_type() const override {return INTEGER;}
};

/* real type */
struct RealValue : TableValue
{
	double real_val;

	RealValue(double d) : real_val(d){}

	ColumnType get_type() const override {return REAL;}
};

/* string type */
struct StringValue : TableValue
{
	std::array<char, 50> str_val;

	StringValue(const char* s) : str_val()
	{
		std::copy(s, s + 50, str_val.begin());
	}

	ColumnType get_type() const override {return STRING;}
};
