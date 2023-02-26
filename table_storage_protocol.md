# Table Storage Protocol
this is a protocol for storing tables in a file.

## table types
as for now, a table might have one of 3 types:
- `INTEGER` - a signed whole number. of 4 bytes.
- `REAL` - a real number of 8 bytes
- `STRING` - a string of pre-defined length.

an unknown length string is right now not implemented.

## Table Protocol
a table protocol will be as follows:

`TABDEF<metadata><table>`

### `TABDEF`
a table file should start with `TABDEF` so we can be sure it aint corrupted or something.

### `metadata`
the metadata is as follows:

`<4-bytes columns count><8-bytes rows count><columns>`

##### `columns`
columns appear contagiously. each column is as follows:

`<1-byte descriptor><column name><null character>`

we use the null character here so we can have column name without fixed length.

the descriptor has a bit structrue which contains the type (like 2 bits) and further data such as whether it's a primary key, etc.

after the last column, the metadata ends.

### `table`
the actual table which are just rows contagiously.
we can allow this since all data types are fixed amount of bytes.

for example:
`<row1-column1><row1-column2><row2-column1><row2-column2> ...`
