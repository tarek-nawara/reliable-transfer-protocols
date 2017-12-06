/** File: main.cpp
 *  Description: entry point for alternating_client
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "StopWaitClient.h"

void init_server_addr(sockaddr_in &server_addr);

int main() {
    int server_socket = utils::socket_wrapper();
    struct sockaddr_in server_addr;
    init_server_addr(server_addr);
    StopWaitClient client{server_socket, server_addr};
    std::string filename = "hamada.txt";
    client.request_file(filename, server_socket, server_addr);
    return 0;
}

void init_server_addr(sockaddr_in &server_addr) {
    memset(&server_addr, 0, sizeof(server_addr));    /* Zero out structure */
    server_addr.sin_family = AF_INET;                 /* Internet addr family */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Server IP address */
    server_addr.sin_port = htons(5000);     /* Server port */
}
