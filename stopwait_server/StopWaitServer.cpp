/**
 *  File: StopWaitServer.cpp
 *  Description: holder for implementation of stop and wait protocol
 *  Created at: 2017-12-1
 */

#include "StopWaitServer.h"

StopWaitServer::StopWaitServer(int server_socket) {
    this->server_socket = server_socket;
}


void
StopWaitServer::handle_client() {
    // TODO
}

void
StopWaitServer::receive_client_request() {
    auto *request_packet = new utils::Packet();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, request_packet, sizeof(*request_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
            std::cout << "[receive_client_request]--- Packet seqno=" << request_packet->seqno << '\n';
            std::cout << "[receive_client_request]--- Packet len=" << request_packet->len << '\n';
            std::cout << "[receive_client_request]--- Packet data=";
            handle_sending_file(request_packet);
            std::cout << '\n';
        }
    }
}

void
StopWaitServer::handle_sending_file(utils::Packet *request_packet) {
    std::ifstream input_stream;
    input_stream.open(request_packet->data);
    long file_size = utils::file_size(std::string(request_packet->data));
    long chunk_count = file_size / PACKET_LEN + (file_size % PACKET_LEN != 0);
    if (chunk_count == 0) {
        return;
    }
    auto res = handle_sending_packet_zero(input_stream);
    auto current_state = res.first;
    auto *packet = res.second;
    --chunk_count;
    while (chunk_count > 0) {
        switch (current_state) {
            case StopWaitServer::State::WAIT_FOR_ACK_ZERO:
                current_state = handle_wait_for_ack_zero(packet);
                break;
            case StopWaitServer::State::WAIT_FOR_ACK_ONE:
                current_state = handle_wait_for_ack_one(packet);
                break;
            case StopWaitServer::State::SENDING_PACKET_ZERO:
                auto res_zero = handle_sending_packet_zero(input_stream);;
                current_state = res_zero.first;
                packet = res_zero.second;
                --chunk_count;
                break;
            case StopWaitServer::State::SENDING_PACKET_ONE:
                auto res_one = handle_sending_packet_one(input_stream);;
                current_state = res_one.first;
                packet = res_one.second;
                --chunk_count;
                break;
        }
    }
}

StopWaitServer::State
StopWaitServer::handle_wait_for_ack_zero(utils::Packet *packet_to_send) {
    return handle_wait_for_ack(packet_to_send, 0, StopWaitServer::State::SENDING_PACKET_ONE);
}

StopWaitServer::State
StopWaitServer::handle_wait_for_ack_one(utils::Packet *packet_to_send) {
    return handle_wait_for_ack(packet_to_send, 1, StopWaitServer::State::SENDING_PACKET_ZERO);
}

std::pair<StopWaitServer::State, utils::Packet *>
StopWaitServer::handle_sending_packet_zero(std::ifstream &input_stream) {
    return handle_sending_packet(input_stream, 0, StopWaitServer::State::WAIT_FOR_ACK_ZERO);
}

std::pair<StopWaitServer::State, utils::Packet *>
StopWaitServer::handle_sending_packet_one(std::ifstream &input_stream) {
    return handle_sending_packet(input_stream, 1, StopWaitServer::State::WAIT_FOR_ACK_ONE);
}

StopWaitServer::State
StopWaitServer::handle_wait_for_ack(utils::Packet *packet_to_send, int ack_no, StopWaitServer::State next_state) {
    auto *ack_packet = new utils::AckPacket();
    clock_t start_time = clock();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, ack_packet, sizeof(*ack_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0 && ack_packet->seqno == ack_no) {
            return next_state;
        } else {
            clock_t current_time = clock();
            double seconds_passed = (double) (current_time - start_time) / CLOCKS_PER_SEC;
            if (seconds_passed >= TIMEOUT) {
                send_packet(packet_to_send);
                start_time = current_time;
            }
        }
    }
}


std::pair<StopWaitServer::State, utils::Packet *>
StopWaitServer::handle_sending_packet(std::ifstream &input_stream, uint32_t packet_no,
                                      StopWaitServer::State next_state) {
    auto *packet = new utils::Packet();
    auto *buffer = new char[PACKET_LEN];
    input_stream.read(buffer, PACKET_LEN);
    int len = 0;
    for (; len < PACKET_LEN && buffer[len] != '\0'; ++len) {
        packet->data[len] = buffer[len];
    }
    packet->len = len;
    packet->seqno = packet_no;
    send_packet(packet);
    return {next_state, packet};
}

void
StopWaitServer::send_packet(utils::Packet *packet) {
    utils::sendto_wrapper(server_socket, packet, sizeof(packet) + sizeof(packet->data),
                          (struct sockaddr *) &client_addr, sizeof(client_addr));
    std::cout << "[send_packet]---Send packet successfully" << '\n';
}

