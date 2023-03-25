"""
the python API for a beekDB server
"""

import socket
import struct

class BeekConnectionError(Exception):
    pass

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
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        # connect to server
        self.__sock.connect(self.__addr)

        # send join message
        self.__send_message(BeekDbConnection.COMMANDS["client_join"], b'')

        # wait for join confirmation
        cmd, _ = self.__recv_message()
        if cmd != BeekDbConnection.COMMANDS["server_join_success"]:
            raise BeekConnectionError("something went wrong")

        # ok

    def query(self, sql_query: str) -> tuple[bool, str | list | None]:
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

        self.__sock.send(msg)

    def __recv_message(self) -> tuple[bytes, bytes]:
        """
        recieves a message from the server. returns a tuple with the command and the content.
        """
        cmd, content_length = struct.unpack("<cQ", self.__sock.recv(9))
        content = self.__sock.recv(content_length)

        return cmd, content

    def close(self) -> None:
        """
        closes the connection with the server
        """

        self.__send_message(BeekDbConnection.COMMANDS['client_leave'], b'')
        self.__sock.close()

    def __enter__(self):
        """
        with statement support
        """
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        with statement support
        """
        self.close()
