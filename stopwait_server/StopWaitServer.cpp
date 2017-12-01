//
// Created by tarek on 12/1/17.
//

#include "StopWaitServer.h"

void StopWaitServer::handle_client() {

}

void StopWaitServer::receive_client_request(int server_socket, sockaddr_in &client_addr) {
    auto *packet = new utils::Packet();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, packet, sizeof(*packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
	    std::cout << "[receive_client_request]--- Packet seqno=" << packet->seqno << '\n';
	    std::cout << "[receive_client_request]--- Packet len=" << packet->len << '\n';
            std::cout << "[receive_client_request]--- Packet data=";
            for (int i = 0; i < packet->len; ++i) {
                std::cout << packet->data[i];
            }
	    std::cout << '\n';
            return;
        }
    }
}

