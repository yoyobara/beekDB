#!/bin/python3
import socket
import time

x = 5

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 1337))

time.sleep(3)

s.send('c'.encode('ascii') + x.to_bytes(8, 'little') + 'hello'.encode('ascii'))

time.sleep(3)

s.close()
