/**
 * File: StopWaitClient.h
 * Description: interface for the stop and wait protocol.
 * Created at: 2017-12-1
 */

#include "StopWaitClient.h"

void
StopWaitClient::get_file(std::string &requested_file, std::string &destination_path, int server_socket) {
    // TODO
}

void
StopWaitClient::request_file(std::string &filename, int server_socket, struct sockaddr_in server_addr) {
    auto *packet = new utils::Packet();
    std::string echo_string = "hello ya mdy7a";
    for (int i = 0; i < echo_string.size(); ++i) {
        packet->data[i] = echo_string[i];
    }
    packet->seqno = 0;
    packet->len = echo_string.size();
    utils::sendto_wrapper(server_socket, packet, sizeof(packet) + sizeof(packet->data), (struct sockaddr *)&server_addr, sizeof(server_addr));
    std::cout << "[request_file]---Send packet successfully" << '\n';
}

void
StopWaitClient::send_ack(int server_socket) {
    // TODO
}
