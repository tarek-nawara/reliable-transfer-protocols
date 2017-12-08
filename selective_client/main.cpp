/**
 *  File: main.cpp
 *  Description: entry point for the selective client
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "SelectiveClient.h"

#define CON_TIME_OUT 1

int main() {
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;
    auto params = utils::read_parameters("client.in");
    utils::init_server_addr(server_addr, std::stoi((*params)[0]), std::stoi((*params)[1]));
    utils::set_connection_time_out(server_socket, CON_TIME_OUT);
    SelectiveClient client{server_socket, server_addr, std::stod((*params)[2]), std::stoi((*params)[3])};
    std::string filename = (*params)[4];
    client.request_file(filename);
    return 0;
}
