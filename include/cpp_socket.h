#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <string_view>
#include <sys/socket.h>

/**
 * socket connection exception
 */
class socket_error : public std::exception
{
	const char* msg;

	public:
		socket_error(const char* message);
		const char * what();
};


/**
 * an abstraction of C sockets to use with C++
 * 
 */
class Socket
{
	public:
		/* create new socket */
		Socket();
		
		/* destructs a socket */
		virtual ~Socket();

		/* binds server to ip and port */
		void bind(uint16_t port) const;

		/* listen to connections */
		void listen(int backlog) const;

		/* accept connection */
		Socket accept() const;

		/* connect to server */
		void connect(const std::string& ip_address, uint16_t port) const; 

	protected:
		/* socket file descriptor */
		int m_sockfd;

		/* construct from existing socket */
		Socket(int fd);
};
