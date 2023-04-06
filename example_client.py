#!/bin/python3

import beeklib

with beeklib.BeekDbConnection('127.0.0.1') as conn:
    print(conn.query("select name, age from yoyo_table;"))
