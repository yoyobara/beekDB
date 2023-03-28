#!/bin/python3

import beeklib

conn = beeklib.BeekDbConnection('127.0.0.1')

print("succ")
conn.close()
