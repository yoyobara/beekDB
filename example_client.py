#!/bin/python3

import beeklib

DEFAULT_PORT = 1337

with beeklib.BeekDbConnection('127.0.0.1', DEFAULT_PORT) as conn:
    while True:
        print(conn.query(input(">>> ")))
