#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <list>
#include <thread>
#include <signal.h>

#include <spdlog/spdlog.h>
#include <openssl/ssl.h>

#include "communication_protocol.h"
#include "client_handler.h"
#include "logging.h"
#include "tables_loader.h"
#include "utils.h"

// threading and ssl
std::list<std::thread> running_client_threads;
SSL_CTX* ssl_context;

int create_socket(uint16_t port)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	int enable = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	if (sock < 0)
		spdlog::get("network")->critical("socket creation error: {}", std::strerror(errno));

	if (bind(sock, (struct sockaddr*)&addr, sizeof addr) < 0 ) 
		spdlog::get("network")->critical("socket bind error: {}", std::strerror(errno));

	if (listen(sock, 1))
		spdlog::get("network")->critical("socket listen error: {}", std::strerror(errno));

	spdlog::get("network")->info("listening to connections on port {} ...", port);

	return sock;
}

uint16_t get_port_from_arg(int argc, char* argv[])
{
	if (argc == 1) // no port given
		return comms_constants::DEFAULT_PORT;
	
	return atoi(argv[1]);
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

int main(int argc, char* argv[])
{
	setup_logger();

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	ssl_context = SSL_CTX_new(TLS_server_method());

	// TODO not error safe
	SSL_CTX_use_certificate_file(ssl_context, "cert/cert.pem", SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(ssl_context, "cert/key.pem", SSL_FILETYPE_PEM);

	int server_fd = create_socket(get_port_from_arg(argc, argv));

	signal(SIGINT, handle_sigint);

    // pre-load tables
    TablesLoader::get_instance();

	// repeatedly accept clients, handle them in seperate threads.
	while (true)
	{
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd < 0)
			spdlog::get("network")->critical("accept error: {}", std::strerror(errno));

		SSL *ssl = SSL_new(ssl_context);
		SSL_set_fd(ssl, client_fd);

		if (SSL_accept(ssl) != 1)
		{
			SSL_shutdown(ssl);
			SSL_free(ssl);
			close(client_fd);
			spdlog::get("network")->critical("ssl accept error: {}", ssl_utils::get_ssl_error());
		}

		// add new ClientThread to the running threads (with the socket)
		running_client_threads.emplace_back(ClientThread(ssl));
	}
}
