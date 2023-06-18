#!/bin/python3
import beeklib

with beeklib.connect('localhost') as c:
    while 1:
        print(c.query("select * from m"))
