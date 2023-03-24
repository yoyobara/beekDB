"""
the python API for a beekDB server
"""

import socket
import struct

class BeekDbConnection:

    COMMANDS = {
        "client_join": b'j',
        "client_leave": b'l',
        "client_query": 'q',
        "server_join_success": b'J',
        "server_query_result": b'Q',
        "server_termination": b'T'
    }

    SIGNATURE = b'TABDEF'

    def __init__(self, ip_address: str, port: int = 1337):
        """
        creates a new connection object.
        
        connecting and joining to the beekDB server.
        """
        self.__addr = (ip_address, port)
        self.__client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self.__client_socket.connect(self.__addr)

    def query(sql_query: str) -> tuple[bool, (str | list | None)]:
        """
        queries the database with an sql query.

        might result in one of several results:
            1. query is ok, no output, in this case (True, None)
            2. query is ok, with output clause (True, <table list>)
            3. query is NOT ok. (False, <error message>)
        """

    def __send_message(self, cmd: bytes, content: bytes) -> None:
        """
        sends a message to the server using the protocol
        """

        content_length = len(content)
        msg = struct.pack(f"<cQ{content_length}s", cmd, content_length, content)

        self.__client_socket.send(msg)

    def __recv_message(self) -> tuple[bytes, bytes]:
        """
        recieves a message from the server. returns a tuple with the command and the content.
        """
        cmd, content_length = struct.unpack("<cQ", self.__client_socket.recv(9))
        content = self.__client_socket.recv(content_length)

        return cmd, content
