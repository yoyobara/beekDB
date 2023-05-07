#include <asm-generic/socket.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <spdlog/common.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <csignal>
#include <atomic>

#include <spdlog/spdlog.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "communication_protocol.h"
#include "client_handler.h"
#include "table/table.h"
#include "table/types.h"
#include "tables_loader.h"

std::vector<std::thread> running_client_threads;

SSL_CTX* ssl_context;

int create_socket()
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(1337);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	int enable = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	if (sock < 0)
		spdlog::critical("socket creation error");

	if (bind(sock, (struct sockaddr*)&addr, sizeof addr) < 0 ) 
		spdlog::critical("socket bind error");

	if (listen(sock, 1))
		spdlog::critical("socket listen error");

	spdlog::info("listening to connections..");

	return sock;
}

/**
 * handle ctrl+c (sigint) to close and cleanup the threads.
 * callback function.
 */
void handle_sigint(int dummy)
{
	// tell threads to stop
	ClientThread::program_running = false;

	// wait for them all to join
	for (std::thread& t : running_client_threads) {
		t.join();
	}

	SSL_CTX_free(ssl_context);
	exit(EXIT_SUCCESS);
}

void setup_logger()
{
	spdlog::set_level(spdlog::level::debug);
	spdlog::set_pattern("[thread %t] %v");
}

int main()
{
	setup_logger();

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	ssl_context = SSL_CTX_new(TLS_server_method());

	// TODO not error safe
	SSL_CTX_use_certificate_file(ssl_context, "cert/cert.pem", SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(ssl_context, "cert/key.pem", SSL_FILETYPE_PEM);

	int server_fd = create_socket();

	signal(SIGINT, handle_sigint);

	// repeatedly accept clients, handle them in seperate threads.
	while (true)
	{
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd < 0)
			spdlog::error("accept error");

		SSL *ssl = SSL_new(ssl_context);
		SSL_set_fd(ssl, client_fd);

		if (SSL_accept(ssl) != 1)
		{
			SSL_shutdown(ssl);
			SSL_free(ssl);
			close(client_fd);

			char buff[250];
			ERR_error_string(ERR_get_error(), buff);
			spdlog::critical("{}", buff);
		}

		spdlog::debug("here?");

		// add new ClientThread to the running threads (with the socket)
		ClientThread t(ssl);
		running_client_threads.emplace_back(t);
	}
}
