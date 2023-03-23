#!/bin/python3
import socket
import time
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 1337))

content = b'select name from mama_table'
l = len(content)

s.send(struct.pack(f"<6scQ{l}s", b'TABDEF', b'q', l, content))
input()
s.close()
