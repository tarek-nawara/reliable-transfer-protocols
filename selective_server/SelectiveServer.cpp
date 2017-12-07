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
}

void SelectiveServer::handle_client_request() {
    auto *request_packet = new utils::Packet();
    while (true) {
        socklen_t client_add_size = sizeof(client_addr);
        ssize_t recv_res = recvfrom(server_socket, request_packet, sizeof(*request_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
            std::cout << "[handle_client_request]--- Packet seqno=" << request_packet->seqno << '\n';
            std::cout << "[handle_client_request]--- Packet len=" << request_packet->len << '\n';
            std::cout << "[handle_client_request]--- Packet data=" << std::string(request_packet->data) << '\n';
            handle_sending_file(request_packet);
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
    auto *ack_packet = new utils::AckPacket();
    socklen_t client_add_size = sizeof(client_addr);
    std::cout << "[handle_sending_file]---File size=" << file_size << '\n';
    std::cout << "[handle_sending_file]---Chunk count=" << chunk_count << '\n';
    while (chunk_count > 0) {
        if (sent_count < WINDOW_SIZE) {
            chunk_count = handle_send_state(input_stream, chunk_count);
        }
        ssize_t recv_res = recvfrom(server_socket, ack_packet, sizeof(*ack_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
            handle_ack(ack_packet);
        }
        packet_clean_up();
    }
}

void
SelectiveServer::packet_clean_up() {
    for (uint32_t i = base; i <= next_seqno; i = (i + 1) % WINDOW_SIZE) {
        if (window[i] == nullptr) continue;
        double seconds_passed = difftime(time(nullptr), window[i]->sent_time);
        if (seconds_passed >= TIME_OUT_DURATION) {
            send_packet_with_prob(window[i]->packet);
            window[i]->sent_time = time(nullptr);
        }
    }
}

void
SelectiveServer::handle_ack(utils::AckPacket *packet) {
    window[packet->seqno] = nullptr;
    --sent_count;
    while (window[base] == nullptr) {
        base = (base + 1) % WINDOW_SIZE;
    }
}

long
SelectiveServer::handle_send_state(std::ifstream &input_stream, long chunk_count) {
    time_t sent_time = time(nullptr);
    auto *packet = create_packet(input_stream);
    window[next_seqno] = new SentPacket(packet, sent_time);
    send_packet_with_prob(packet);
    next_seqno = (next_seqno + 1) % WINDOW_SIZE;
    ++sent_count;
    --chunk_count;
    return chunk_count;
}

utils::Packet *
SelectiveServer::create_packet(std::ifstream &input_stream) {
    auto *packet = new utils::Packet();
    auto *buffer = new char[PACKET_LEN];
    input_stream.read(buffer, PACKET_LEN);
    uint16_t len = 0;
    for (; len < PACKET_LEN && buffer[len] != '\0'; ++len) {
        packet->data[len] = buffer[len];
    }
    packet->len = len;
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
    return ((double) rand() / RAND_MAX) > this->plp;
}



