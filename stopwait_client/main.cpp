/** File: main.cpp
 *  Description: entry point for alternating_client
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "StopWaitClient.h"

void init_server_addr(sockaddr_in &server_addr);
void set_connection_time_out(int server_socket);

int main() {
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;
    init_server_addr(server_addr);
    set_connection_time_out(server_socket);
    StopWaitClient client{server_socket, server_addr};
    std::string filename = "hello.txt";
    client.request_file(filename);
    return 0;
}

void init_server_addr(sockaddr_in &server_addr) {
    memset(&server_addr, 0, sizeof(server_addr));    /* Zero out structure */
    server_addr.sin_family = AF_INET;                 /* Internet addr family */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Server IP address */
    server_addr.sin_port = htons(5000);     /* Server port */
}

void set_connection_time_out(int server_socket) {
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }
}
