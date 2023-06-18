#!/bin/python3
"""
usage example of the beeklib module
"""

import beeklib
import random

# connect to client
with beeklib.connect("localhost") as conn:

    # create table
    conn.query("create table m(x integer, y integer)")

    # insert random values forever
    while 1:
        conn.query(f'insert into m values({random.randint(1, 10), random.randint(1, 20)})')

