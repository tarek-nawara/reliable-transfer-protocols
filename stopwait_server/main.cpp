/** File: main.cpp
 *  Description: entry point for alternating_server
 *  Created at: 2017-12-1
 */

#include "socket_utils.h"
#include "StopWaitServer.h"

sockaddr_in &init_server_addr(sockaddr_in &echoServAddr);

int main() {
//    int server_socket = utils::socket_wrapper();
//    struct sockaddr_in server_addr; /* Local address */
//    struct sockaddr_in echoClntAddr; /* Client address */
//
//    server_addr = init_server_addr(server_addr);
//    utils::bind_wrapper(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
//    std::cout << "---Server start---" << '\n';
//    std::cout << "---Socket number=" << server_socket << '\n';
//    StopWaitServer server;
//    server.receive_client_request(server_socket, echoClntAddr);
//    return 0;
    std::ifstream input_stream;
    input_stream.open("hello.txt");
    char *buffer = new char[2];
    long file_size = utils::file_size("hello.txt");
    long counts = file_size / 2 + (file_size % 2 != 0);
    while (counts > 0) {
        input_stream.read(buffer, 2);
        std::cout << "[main]---Buffer=" << std::string(buffer) << '\n';
        --counts;
    }
}

sockaddr_in &init_server_addr(sockaddr_in &echoServAddr) {
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(5000);      /* Local port */
    return echoServAddr;
}
