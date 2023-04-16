#pragma once
#include <algorithm>
#include <array>
#include <cstring>
#include <string>

/* the three possible column types */
enum ColumnType
{
	INTEGER,
	REAL,
	VARCHAR_50
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
	IntegerValue(const char* raw) : int_val(*reinterpret_cast<const int*>(raw)) {}

	ColumnType get_type() const override {return INTEGER;}
};

/* real type */
struct RealValue : TableValue
{
	double real_val;

	RealValue(double d) : real_val(d){}
	RealValue(const char* raw) : real_val(*reinterpret_cast<const double*>(raw)) {}

	ColumnType get_type() const override {return REAL;}
};

/* string type */
struct VarChar50Value : TableValue
{
	std::array<char, 50> str_val;

	VarChar50Value(const char* raw) : str_val{}
	{
		std::strcpy(str_val.data(), raw);
	}

	ColumnType get_type() const override {return VARCHAR_50;}
};
