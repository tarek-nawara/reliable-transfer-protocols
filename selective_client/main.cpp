/** File: main.cpp
 *  Description: entry point for the selective client
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "SelectiveClient.h"

void init_server_addr(sockaddr_in &server_addr);
void set_connection_time_out(int server_socket);

int main() {
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;
    init_server_addr(server_addr);
    set_connection_time_out(server_socket);
    SelectiveClient client{server_socket, server_addr};
    std::string filename = "hello.txt";
    client.request_file(filename);
    return 0;
}

void init_server_addr(sockaddr_in &server_addr) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5000);
}

void set_connection_time_out(int server_socket) {
    struct timeval tv = {1, 0};
    if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }
}