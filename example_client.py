#!/bin/python3

import beeklib

with beeklib.BeekDbConnection('127.0.0.1') as conn:
    print(conn.query("create table tab (col1 INTEGER, col2 REAL, col3 VARCHAR(50));"))
