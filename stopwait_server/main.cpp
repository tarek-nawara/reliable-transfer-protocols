/** File: main.cpp
 *  Description: entry point for alternating_server
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "StopWaitServer.h"

sockaddr_in &init_server_addr(sockaddr_in &server_addr);
void set_connection_time_out(int server_socket);

int main() {
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;

    server_addr = init_server_addr(server_addr);
    utils::bind_wrapper(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    set_connection_time_out(server_socket);
    std::cout << "---Server start---" << '\n';
    std::cout << "---Socket number=" << server_socket << '\n';
    StopWaitServer server{server_socket, 0.5, 20};
    server.handle_client_request();
    return 0;
}

sockaddr_in &init_server_addr(sockaddr_in &server_addr) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5000);
    return server_addr;
}

void set_connection_time_out(int server_socket) {
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }
}
