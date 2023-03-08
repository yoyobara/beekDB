#!/bin/python3
import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('127.0.0.1', 1337))
x = 5

time.sleep(3)

s.send(b"hello")

time.sleep(3)

s.close()
