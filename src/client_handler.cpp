#include "client_handler.h"
#include "communication_protocol.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

std::vector<ClientThread> ClientThread::running_client_threads;
std::atomic<bool> ClientThread::program_running = true;

/* on constructor call, start thread */
ClientThread::ClientThread(Socket client_socket) : 
	m_client(client_socket), 
	m_thread(&ClientThread::run, this) {}

void ClientThread::run()
{
	comms::send_message(m_client, {'a', "hello"});
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "thread finished\n";
}
