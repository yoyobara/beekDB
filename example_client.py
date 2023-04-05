#!/bin/python3

import beeklib

with beeklib.BeekDbConnection('127.0.0.1') as conn:
    print(conn.query("insert into tab (col1, col2, col3) values(55, 2.2, 'hey bitch')"))
