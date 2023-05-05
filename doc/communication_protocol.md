# Communication Protocol

## General

generally, messages between the server and the client will consist of several parts:

`<1-char command> <8 bytes integer of content length> <content>`

the content clause depends on the message's goal, which is determined by the command.
messages can be sent as multiple packets; the command and content length needs to be on the first
packet, but the rest can be delivered by several packets since we know the total content length.

## Client -> Server

### Connection Messages

#### Join (`j`)
when connecting, the client shall send this message so the server will manage the connection.
after sent the client should connect for server acceptance.

also sends as its `content` the client's public key in order to exchange keys using the DH protocol.

#### Leave (`l`)
before disconnecting from the server, the client shall send this message to say that he left
so the server can disconnect him.

has no `content` at all.

### Sql Queries
an sql query can be sent to the server so it'll be executed.
after doing so the client should wait for the query result set.

#### Query (`q`)
send a query to the server.

the content here is the query text itself. limited to a single query.

## Server -> Client

### Connection Messages

#### Join_Success (`J`)
when accepting a new client, the server shall reply with this response after reciving the `Join` message from the client.

also sends as its `content` its public key also as part of the DH protocol.

#### Query_Result (`Q`)
a response message to a query from the client (`Query`)

its content is as follows:

`<1-char result><result set if success / operation error message / empty>`

possible result chars:
- `s` - success. the rest will contains the result set.
- `e` - sql error. content contains error message

the result set is a table, whose serialization is identical to the *storage protocol*.

#### Server_Termination ('T')
this is sent to the client when the serer is manually terminated.

## Diffie-Hellman Key Exchange protocol

### Process
using the `Join` message, the client sends the sever the DH parameters, and its public key. 
the server initializes the DH stuff using the recieved parameters, and then generates its own 
keys and sends the public key to the client

then both sizes calculate the shared key and uses it to communicate
