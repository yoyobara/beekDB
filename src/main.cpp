#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "cpp_socket.h"
#include "table.h"

/*/1** */
/* * a thread running function which is in charge of handling a simple client */
/* *1/ */
/*void handle_client(Socket client) */
/*{ */
/*	auto s {client.recv(10)}; */
/*	std::cout << s << std::endl; */
/*	client.close(); */
/*} */

/*int main() */
/*{ */
/*	Socket server; */
/*	server.bind(1337); */
/*	server.listen(2); */

/*	// a vector containing the currently running threads */
/*	std::vector<std::thread> running_threads; */

/*	// repeatedly accept clients, handle them in seperate threads. */
/*	while (true) */
/*	{ */
/*		Socket client = server.accept(); */
/*		running_threads.push_back(std::thread(handle_client, client)); */
/*	} */

/*	return EXIT_SUCCESS; */
/*} */

int main()
{
	Table t("./tables/sample_table");
}
