/** File: main.cpp
 *  Description: entry point for alternating_server
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "StopWaitServer.h"
#define CON_TIME_OUT 1

int main() {
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;
    auto params = utils::read_parameters("server.in");

    utils::init_server_addr(server_addr, static_cast<uint32_t>(std::stoi((*params)[0])),
                            static_cast<uint16_t>(std::stoi((*params)[1])));
    utils::bind_wrapper(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    utils::set_connection_time_out(server_socket, CON_TIME_OUT);
    std::cout << "---Server start---" << '\n';
    std::cout << "---Socket number=" << server_socket << '\n';
    StopWaitServer server{server_socket, std::stod((*params)[4]), static_cast<unsigned int>(std::stoi((*params)[3]))};
    server.handle_client_request();
    return 0;
}
