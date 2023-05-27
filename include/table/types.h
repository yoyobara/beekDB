#pragma once

#include <array>
#include <cstring>

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
	virtual void* get_value_pointer() = 0;
};

/* integer type value */
struct IntegerValue : TableValue
{

	int int_val;

	IntegerValue(int i) : int_val(i) {}
	IntegerValue(const char* raw) : int_val(*reinterpret_cast<const int*>(raw)) {}

	ColumnType get_type() const override {return INTEGER;}
	void * get_value_pointer() override
	{
		return &int_val;
	}
};

/* real type */
struct RealValue : TableValue
{
	double real_val;

	RealValue(double d) : real_val(d){}
	RealValue(const char* raw) : real_val(*reinterpret_cast<const double*>(raw)) {}

	ColumnType get_type() const override {return REAL;}
	void * get_value_pointer() override
	{
		return &real_val;
	}
};

/* string type */
struct VarChar50Value : TableValue
{
	std::array<char, 50> str_val;

	/* construct from 50 chars */
	VarChar50Value(const char* raw) : str_val{}
	{
		std::strncpy(str_val.data(), raw, 50);
	}

	ColumnType get_type() const override {return VARCHAR_50;}
	void * get_value_pointer() override
	{
		return &str_val;
	}
};
