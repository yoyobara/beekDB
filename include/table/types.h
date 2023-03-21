#pragma once

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
// TODO
//

/* string type */
// TODO
