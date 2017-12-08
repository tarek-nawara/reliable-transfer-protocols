/** File: main.cpp
 *  Description: entry point for alternating_client
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "StopWaitClient.h"
#define CON_TIME_OUT 1

int main() {
    auto res = utils::read_parameters("client.in");
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;
    utils::init_server_addr(server_addr, std::stoi((*res)[0]), std::stoi((*res)[1]));
    utils::set_connection_time_out(server_socket, CON_TIME_OUT);
    StopWaitClient client{server_socket, server_addr};
    std::string filename = "hello.txt";
    client.request_file(filename);
}
