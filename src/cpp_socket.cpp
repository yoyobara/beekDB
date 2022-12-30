#include "cpp_socket.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

// socket error implementation
socket_error::socket_error(const char* message) : msg(message){}
const char* socket_error::what(){ return msg;}

Socket::Socket()
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd < 0) throw socket_error("socket creation error");
}

Socket::Socket(int fd) : m_sockfd(fd) {}

Socket::~Socket()
{
	close(m_sockfd);
}

void Socket::bind(uint16_t port) const
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;

	int res = ::bind(m_sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
	if (res < 0)
		throw socket_error("socket bind error");
}

void Socket::listen(int backlog) const
{
	int res = ::listen(m_sockfd, backlog);
	if (res < 0)
		throw socket_error("socket listen error");
}

Socket Socket::accept() const
{
	struct sockaddr_in client_address;
	socklen_t conn_length = sizeof(client_address);

	int conn = ::accept(m_sockfd, (struct sockaddr*)&client_address, &conn_length);
	if (conn < 0) throw socket_error("accept error");

	return Socket(conn);
}

void Socket::connect(const std::string& ip_address, uint16_t port) const
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip_address.c_str());

	int result = ::connect(m_sockfd, (struct sockaddr*)&addr, sizeof addr);
	if (result < 0)
		throw socket_error("connecting error");
}
