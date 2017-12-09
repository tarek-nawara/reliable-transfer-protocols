/**
 *  File: SelectiveServer.cpp
 *  Description: Implementation for the selective repeat protocol
 *  Created at: 2017-12-8
 */

#include <socket_utils.h>
#include "SelectiveServer.h"


SelectiveServer::SelectiveServer(int server_socket, double plp, unsigned int seed) {
    this->server_socket = server_socket;
    this->plp = plp;
    srand(seed);
    for (size_t i = 0; i < window_size; ++i) {
        this->window[i] = nullptr;
    }
}

SelectiveServer::~SelectiveServer() {
    for (size_t i = 0; i < this->window_size; ++i) {
        delete window[i];
    }
    delete[] window;
}

void
SelectiveServer::handle_client_request() {
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
SelectiveServer::handle_sending_file(utils::Packet *request_packet) {
    std::ifstream input_stream;
    input_stream.open(request_packet->data);
    long file_size = utils::file_size(std::string(request_packet->data));
    long chunk_count = file_size / PACKET_LEN + (file_size % PACKET_LEN != 0);
    if (chunk_count == 0) {
        return;
    }
    send_header_packet(chunk_count, window_size);
    auto ack_packet = std::make_unique<utils::AckPacket>();
    socklen_t client_add_size = sizeof(client_addr);
    std::cout << "[handle_sending_file]---File size=" << file_size << '\n';
    std::cout << "[handle_sending_file]---Chunk count=" << chunk_count << '\n';
    while (chunk_count > 0 || sent_count > 0) {
        if (chunk_count > 0 && window[next_seqno] == nullptr) {
            handle_send_state(input_stream, chunk_count, file_size);
            --chunk_count;
        }
        ssize_t recv_res = recvfrom(server_socket, ack_packet.get(), sizeof(*ack_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
            handle_ack(ack_packet.get());
        }
        sent_count = packet_clean_up();
        std::cout << "[handle_sending_file]---Chunk count=" << chunk_count
                  << " sent_count=" << sent_count << '\n';
    }
}

uint32_t
SelectiveServer::packet_clean_up() {
    uint32_t sent_count = 0;
    uint32_t idx = base;
    for (uint32_t loop_count = 0; loop_count < window_size; ++loop_count, idx = (idx + 1) % window_size) {
        if (window[idx] == nullptr) continue;
        ++sent_count;
        std::cout << "[packet_clean_up]---Waiting for ack of packet number=" << window[idx]->packet->seqno << '\n';
        double seconds_passed = difftime(time(nullptr), window[idx]->sent_time);
        if (seconds_passed >= TIME_OUT_DURATION) {
            std::cout << "[packet_clean_up]---Resending packet number=" << window[idx]->packet->seqno << '\n';
            send_packet_with_prob(window[idx]->packet);
            window[idx]->sent_time = time(nullptr);
        }
    }
    return sent_count;
}

void
SelectiveServer::handle_ack(utils::AckPacket *ack_packet) {
    std::cout << "[handle_ack]---Received ack number=" << ack_packet->seqno << '\n';
    if (window[ack_packet->seqno] == nullptr) return;
    delete window[ack_packet->seqno];
    window[ack_packet->seqno] = nullptr;
    uint32_t loop_count = 0;
    while (window[base] == nullptr && loop_count < window_size) {
        base = (base + 1) % window_size;
        ++loop_count;
    }
}

void
SelectiveServer::handle_send_state(std::ifstream &input_stream, long chunk_count, long file_size) {
    if (window[next_seqno] != nullptr) {
        std::cout << "[handle_send_state]---Error sending before receiving ack" << '\n';
    }
    time_t sent_time = time(nullptr);
    auto *packet = create_packet(input_stream, chunk_count, file_size);
    window[next_seqno] = new SentPacket(packet, sent_time);
    send_packet_with_prob(packet);
    next_seqno = (next_seqno + 1) % window_size;
}

void
SelectiveServer::send_header_packet(long chunk_count, int window_size) {
    auto header_packet = std::make_unique<utils::Packet>();
    header_packet->seqno = static_cast<uint32_t>(chunk_count);
    header_packet->len = static_cast<uint16_t>(window_size);
    send_packet(header_packet.get());
}

utils::Packet *
SelectiveServer::create_packet(std::ifstream &input_stream, long chunk_count, long file_size) {
    int send_len = PACKET_LEN;
    if (chunk_count == 1) {
        send_len = static_cast<int>(file_size % PACKET_LEN);
    }
    auto *packet = new utils::Packet();
    input_stream.read(packet->data, send_len);
    packet->len = static_cast<uint16_t>(send_len);
    packet->seqno = next_seqno;
    return packet;
}

void
SelectiveServer::send_packet(utils::Packet *packet) {
    utils::sendto_wrapper(server_socket, packet, sizeof(packet) + sizeof(packet->data),
                          (struct sockaddr *) &client_addr, sizeof(client_addr));
    std::cout << "[send_packet]---Send packet successfully" << '\n';
}

void
SelectiveServer::send_packet_with_prob(utils::Packet *packet) {
    if (should_send_packet()) {
        std::cout << "[create_packet]--- Sending packet no=" << packet->seqno << '\n';
        send_packet(packet);
    }
}

bool
SelectiveServer::should_send_packet() {
    if (plp <= 1e-5) {
        return true;
    }
    return ((double) rand() / RAND_MAX) > this->plp;
}

