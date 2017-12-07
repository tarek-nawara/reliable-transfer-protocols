/** File: main.cpp
 *  Description: entry point for alternating_server
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "StopWaitServer.h"

sockaddr_in &init_server_addr(sockaddr_in &server_addr);

int main() {
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;

    server_addr = init_server_addr(server_addr);
    utils::bind_wrapper(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    std::cout << "---Server start---" << '\n';
    std::cout << "---Socket number=" << server_socket << '\n';
    StopWaitServer server{server_socket};
    server.handle_client_requests_fork();
    return 0;
}

sockaddr_in &init_server_addr(sockaddr_in &server_addr) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5000);
    return server_addr;
}
