/**
 *  File: main.cpp
 *  Description: entry point for the selective server
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "SelectiveServer.h"
#define CON_TIME_OUT 1

int main() {
	int server_socket = utils::socket_wrapper();
	struct sockaddr_in server_addr;
	auto params = utils::read_parameters("server.in");

	utils::init_server_addr(server_addr, std::stoi((*params)[0]), std::stoi((*params)[1]));
	utils::bind_wrapper(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
	utils::set_connection_time_out(server_socket, CON_TIME_OUT);
	std::cout << "---Server start---" << '\n';
	std::cout << "---Socket number=" << server_socket << '\n';
	SelectiveServer server{server_socket, std::stod((*params)[4]), std::stoi((*params)[3]), std::stoi((*params)[2])};
	server.handle_client_request();
	return 0;
}