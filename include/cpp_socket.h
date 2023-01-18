#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

/**
 * socket connection exception
 */
class socket_error : public std::exception
{
	const char* msg;

	public:
		socket_error(const char* message);
		const char * what() const noexcept override;
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

		/**
		 * binds the server to run on a specific port
		 * and to accept all connections
		 *
		 * @param port - the port to run on,
		 */
		void bind(uint16_t port) const;

		/**
		 * listen to upcoming connections.
		 *
		 * @param backlog - the possible amount of sockets waiting in the queue
		 */
		void listen(int backlog) const;

		/**
		 * accepts a connection. call after bind & listen.
		 *
		 * @return the socket of the accepted connection.
		 */
		Socket accept() const;

		/**
		 * connects to a server.
		 *
		 * @param ip_address - the ip address to connect to.
		 * @param port - the port to connect to.
		 */
		void connect(const std::string& ip_address, uint16_t port) const; 

		/**
		 * send operation.
		 *
		 * @param data - data to send
		 *
		 * @return bytes sent
		 */
		ssize_t send(const std::string& data) const;

		/**
		 * recive operation.
		 *
		 * @param length - how many bytes to recive
		 *
		 * @return the recived data
		 */
		std::string recv(size_t length) const;

	protected:
		/* socket file descriptor */
		int m_sockfd;

		/* construct from existing socket */
		Socket(int fd);
};
