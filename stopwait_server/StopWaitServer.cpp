/**
 *  File: StopWaitServer.cpp
 *  Description: holder for implementation of stop and wait protocol
 *  Created at: 2017-12-1
 */

#include <socket_utils.h>
#include "StopWaitServer.h"

StopWaitServer::StopWaitServer(int server_socket, double plp, unsigned int seed) {
    this->server_socket = server_socket;
    this->plp = plp;
    srand(seed);
}

void
StopWaitServer::handle_client_request() {
    auto request_packet = std::make_unique<utils::Packet>();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, request_packet.get(), sizeof(*request_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
            std::cout << "[handle_client_request]--- Packet seqno=" << request_packet->seqno << '\n';
            std::cout << "[handle_client_request]--- Packet len=" << request_packet->len << '\n';
            std::cout << "[handle_client_request]--- Packet data=" << std::string(request_packet->data) << '\n';
            handle_sending_file(request_packet.get());
            break;
        }
    }
}

void
StopWaitServer::handle_sending_file(utils::Packet *request_packet) {
    std::ifstream input_stream;
    input_stream.open(request_packet->data);
    this->file_size = utils::file_size(std::string(request_packet->data));
    this->chunk_count = file_size / PACKET_LEN + (file_size % PACKET_LEN != 0);
    if (chunk_count == 0) {
        return;
    }
    std::cout << "[handle_sending_file]---File size=" << file_size << '\n';
    std::cout << "[handle_sending_file]---Chunk count=" << chunk_count << '\n';
    auto current_state = StopWaitServer::State::SENDING_PACKET_ZERO;
    std::shared_ptr<utils::Packet> packet = nullptr;
    bool waiting_for_ack = false;
    while (chunk_count > 0 || waiting_for_ack) {
        switch (current_state) {
            case StopWaitServer::State::WAIT_FOR_ACK_ZERO: {
                current_state = handle_wait_for_ack_zero(packet.get());
                waiting_for_ack = false;
                break;
            }
            case StopWaitServer::State::WAIT_FOR_ACK_ONE: {
                current_state = handle_wait_for_ack_one(packet.get());
                waiting_for_ack = false;
                break;
            }
            case StopWaitServer::State::SENDING_PACKET_ZERO: {
                auto res_zero = handle_sending_packet_zero(input_stream);;
                current_state = res_zero.first;
                packet = res_zero.second;
                waiting_for_ack = true;
                --chunk_count;
                break;
            }
            case StopWaitServer::State::SENDING_PACKET_ONE: {
                auto res_one = handle_sending_packet_one(input_stream);;
                current_state = res_one.first;
                packet = res_one.second;
                waiting_for_ack = true;
                --chunk_count;
                break;
            }
        }
    }
    std::cout << "[handle_sending_file]---Sending termination packet" << '\n';
    auto termination_packet = std::make_unique<utils::Packet>();
    termination_packet->seqno = 100;
    termination_packet->len = 0;
    send_packet(termination_packet.get());
}

StopWaitServer::State
StopWaitServer::handle_wait_for_ack_zero(utils::Packet *packet_to_send) {
    return handle_wait_for_ack(packet_to_send, 0, StopWaitServer::State::SENDING_PACKET_ONE);
}

StopWaitServer::State
StopWaitServer::handle_wait_for_ack_one(utils::Packet *packet_to_send) {
    return handle_wait_for_ack(packet_to_send, 1, StopWaitServer::State::SENDING_PACKET_ZERO);
}

std::pair<StopWaitServer::State, std::shared_ptr<utils::Packet>>
StopWaitServer::handle_sending_packet_zero(std::ifstream &input_stream) {
    return handle_sending_packet(input_stream, 0, StopWaitServer::State::WAIT_FOR_ACK_ZERO);
}

std::pair<StopWaitServer::State, std::shared_ptr<utils::Packet>>
StopWaitServer::handle_sending_packet_one(std::ifstream &input_stream) {
    return handle_sending_packet(input_stream, 1, StopWaitServer::State::WAIT_FOR_ACK_ONE);
}

StopWaitServer::State
StopWaitServer::handle_wait_for_ack(utils::Packet *packet_to_send, uint32_t ack_no, StopWaitServer::State next_state) {
    std::cout << "[handle_wait_for_ack]---Waiting for ack number=" << ack_no << '\n';
    auto ack_packet = std::make_unique<utils::AckPacket>();
    time_t start_time = time(0);
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, ack_packet.get(), sizeof(*ack_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        std::cout << "[handle_wait_for_ack]--- Received AckPacket seqno=" << ack_packet->seqno << '\n';
        double seconds_passed = difftime(time(nullptr), start_time);
        std::cout << "[handle_wait_for_ack]---Seconds passed=" << seconds_passed << '\n';
        if (recv_res > 0 && ack_packet->seqno == ack_no) {
            return next_state;
        } else if (seconds_passed >= TIME_OUT_DURATION) {
            std::cout << "[handle_wait_for_ack]---Timeout, trying to send the packet again, packet number="
                      << packet_to_send->seqno << '\n';
            send_packet_with_prob(packet_to_send);
            start_time = time(nullptr);
        }
    }
}


std::pair<StopWaitServer::State, std::shared_ptr<utils::Packet>>
StopWaitServer::handle_sending_packet(std::ifstream &input_stream, uint32_t packet_no,
                                      StopWaitServer::State next_state) {
    int sent_len = PACKET_LEN;
    if (chunk_count == 1) {
        sent_len = static_cast<int>(file_size % PACKET_LEN);
    }
    auto packet = std::make_shared<utils::Packet>();
    input_stream.read(packet->data, sent_len);
    packet->len = static_cast<uint16_t>(sent_len);
    packet->seqno = packet_no;
    send_packet_with_prob(packet.get());
    return {next_state, packet};
}

void
StopWaitServer::send_packet(utils::Packet *packet) {
    utils::sendto_wrapper(server_socket, packet, sizeof(packet) + sizeof(packet->data),
                          (struct sockaddr *) &client_addr, sizeof(client_addr));
    std::cout << "[send_packet]---Send packet successfully" << '\n';
}

void
StopWaitServer::send_packet_with_prob(utils::Packet *packet) {
    if (should_send_packet()) {
        std::cout << "[handle_sending_packet]--- Sending packet no=" << packet->seqno << '\n';
        send_packet(packet);
    }
}

bool StopWaitServer::should_send_packet() {
    return ((double) rand() / RAND_MAX) > this->plp;
}
