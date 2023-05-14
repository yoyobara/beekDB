#!/bin/python3

import beeklib

with beeklib.BeekDbConnection('127.0.0.1', 2222) as conn:
    while True:
        print(conn.query(input(">>> ")))
