/**
 *  File: StopWaitServer.cpp
 *  Description: holder for implementation of stop and wait protocol
 *  Created at: 2017-12-1
 */

#include "StopWaitServer.h"

void StopWaitServer::handle_client() {
    // TODO
}

void
StopWaitServer::receive_client_request(int server_socket, sockaddr_in &client_addr) {
    auto *request_packet = new utils::Packet();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, request_packet, sizeof(*request_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
            std::cout << "[receive_client_request]--- Packet seqno=" << request_packet->seqno << '\n';
            std::cout << "[receive_client_request]--- Packet len=" << request_packet->len << '\n';
            std::cout << "[receive_client_request]--- Packet data=";
            handle_sending_file(server_socket, client_addr, request_packet);
            std::cout << '\n';
        }
    }
}

void
StopWaitServer::handle_sending_file(int server_socket, sockaddr_in &client_addr, utils::Packet *request_packet) {
    std::ifstream input_stream;
    input_stream.open(request_packet->data);
    long file_size = utils::file_size(std::string(request_packet->data));
    long chunk_count = file_size / WINDOW_SIZE + (file_size % WINDOW_SIZE != 0);
    if (chunk_count == 0) {
        return;
    }
    auto res = handle_sending_packet_zero(server_socket, client_addr, input_stream);
    auto current_state = res.first;
    auto packet = res.second;
    --chunk_count;
    while (chunk_count > 0) {
        switch (current_state) {
            case StopWaitServer::State::WAIT_FOR_ACK_ZERO:
                current_state = handle_wait_for_ack_zero(server_socket, client_addr, packet);
                break;
            case StopWaitServer::State::WAIT_FOR_ACK_ONE:
                current_state = handle_wait_for_ack_one(server_socket, client_addr, packet);
                break;
            case StopWaitServer::State::SENDING_PACKET_ZERO:
                auto res_zero = handle_sending_packet_zero(server_socket, client_addr, input_stream);;
                current_state = res_zero.first;
                packet = res_zero.second;
                --chunk_count;
                break;
            case StopWaitServer::State::SENDING_PACKET_ONE:
                auto res_one = handle_sending_packet_one(server_socket, client_addr, input_stream);;
                current_state = res_one.first;
                packet = res_one.second;
                --chunk_count;
                break;
        }
    }
}

StopWaitServer::State
StopWaitServer::handle_wait_for_ack_zero(int server_socket, sockaddr_in &client_addr, utils::Packet *packet_to_send) {
    set_connection_time_out(server_socket);
    auto *ack_packet = new utils::AckPacket();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, ack_packet, sizeof(*ack_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0 && ack_packet->seqno == 0) { // TODO handle timeout
            return StopWaitServer::State::SENDING_PACKET_ONE;
        } else if (recv_res > 0) {
            send_packet(server_socket, client_addr, packet_to_send);
        }
    }
}

std::pair<StopWaitServer::State, utils::Packet *>
StopWaitServer::handle_sending_packet_zero(int server_socket, sockaddr_in &client_addr, std::ifstream &input_stream) {
    auto *packet = new utils::Packet();
    auto *buffer = new char[WINDOW_SIZE];
    input_stream.read(buffer, WINDOW_SIZE);
    int len = 0;
    for (; len < WINDOW_SIZE && buffer[len] != '\0'; ++len) {
        packet->data[len] = buffer[len];
    }
    packet->len = len;
    packet->seqno = 0;
    send_packet(server_socket, client_addr, packet);
    return {StopWaitServer::State::WAIT_FOR_ACK_ZERO, packet};
}


StopWaitServer::State
StopWaitServer::handle_wait_for_ack_one(int server_socket, sockaddr_in &client_addr, utils::Packet *packet_to_send) {
    set_connection_time_out(server_socket);
    auto *ack_packet = new utils::AckPacket();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, ack_packet, sizeof(*ack_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0 && ack_packet->seqno == 1) { // TODO should handle timeout
            return StopWaitServer::State::SENDING_PACKET_ZERO;
        } else if (recv_res > 0) {
            send_packet(server_socket, client_addr, packet_to_send);
        }
    }
}

std::pair<StopWaitServer::State, utils::Packet *>
StopWaitServer::handle_sending_packet_one(int server_socket, sockaddr_in &client_addr, std::ifstream &input_stream) {
    auto *packet = new utils::Packet();
    auto *buffer = new char[WINDOW_SIZE];
    input_stream.read(buffer, WINDOW_SIZE);
    int len = 0;
    for (; len < WINDOW_SIZE && buffer[len] != '\0'; ++len) {
        packet->data[len] = buffer[len];
    }
    packet->len = len;
    packet->seqno = 1;
    send_packet(server_socket, client_addr, packet);
    return {StopWaitServer::State::WAIT_FOR_ACK_ONE, packet};
}

void
StopWaitServer::set_connection_time_out(int server_socket) {
    struct timeval tv = {1, 0};
    if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error");
    }
}

void
StopWaitServer::send_packet(int server_socket, sockaddr_in &client_addr, utils::Packet *packet) {
    utils::sendto_wrapper(server_socket, packet, sizeof(packet) + sizeof(packet->data),
                          (struct sockaddr *) &client_addr, sizeof(client_addr));
    std::cout << "[send_packet]---Send packet successfully" << '\n';
}

