/**
 *  File: SelectiveServer.cpp
 *  Description: Implementation for the selective repeat protocol
 *  Created at: 2017-12-8
 */

#include "SelectiveServer.h"

SelectiveServer::SelectiveServer(int server_socket, double plp, unsigned int seed, uint32_t max_window_size) {
    this->server_socket = server_socket;
    this->plp = plp;
    srand(seed);
    this->max_window_size = max_window_size;
    std::cout << "max_window_size=" << max_window_size << '\n';
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
            handle_sending_file(*request_packet);
            break;
        }
    }
}

void
SelectiveServer::handle_sending_file(utils::Packet &request_packet) {
    std::ifstream input_stream;
    input_stream.open(request_packet.data);
    auto file_size = utils::file_size(std::string(request_packet.data));
    auto chunk_count = file_size / PACKET_LEN + (file_size % PACKET_LEN != 0);
    if (chunk_count == 0) {
        return;
    }
    send_header_packet(chunk_count);
    auto ack_packet = std::make_unique<utils::AckPacket>();
    socklen_t client_add_size = sizeof(client_addr);
    std::cout << "[handle_sending_file]---File size=" << file_size << '\n';
    std::cout << "[handle_sending_file]---Chunk count=" << chunk_count << '\n';
    std::cout << "[handle_sending_file]---Initial window size=" << window_size << '\n';

    while (chunk_count > 0 || !window.empty()) {
        if (chunk_count > 0 && window.size() < window_size) {
            handle_send_state(input_stream, chunk_count, file_size);
            --chunk_count;
        }
        ssize_t recv_res = recvfrom(server_socket, ack_packet.get(), sizeof(*ack_packet), 0,
                                    (struct sockaddr *) &client_addr, &client_add_size);
        if (recv_res > 0) {
            handle_ack(*ack_packet);
        }
        auto timed_out_packets = packet_clean_up();
        std::cout << "finished packet clean up" << '\n';
        std::cout << "[handle_sending_file]---Chunk count=" << chunk_count
                  << " sent_count=" << window.size() << " window_size=" << window_size << "timed_out_packets="
                  << timed_out_packets << '\n';

        if (timed_out_packets == 0 && window_size < max_window_size) {
            ++window_size;
        } else if (timed_out_packets > 0 && window_size > 1) {
            window_size /= 2;
        }
    }
}

uint32_t
SelectiveServer::packet_clean_up() {
    uint32_t timed_out_packets = 0;
    while (!pq.empty()) {
        auto packet_time = pq.top().first;
        auto packet_number = pq.top().second;
        if (window.find(packet_number) == window.end()) {
            std::cout << "[packet_clean_up]---Not in window packet number=" << packet_number << '\n';
            pq.pop();
            continue;
        }

        double seconds_passed = difftime(time(nullptr), packet_time);
        std::cout << "[packet_clean_up]---Seconds passed=" << seconds_passed << '\n';
        if (seconds_passed < TIME_OUT_DURATION) break;

        ++timed_out_packets;
        time_t sent_time = time(nullptr);
        std::cout << "[packet_clean_up]---Resending packet number=" << window[packet_number]->seqno << '\n';
        send_packet_with_prob(*window[packet_number]);
        pq.pop();
        pq.push({sent_time, packet_number});
    }
    return timed_out_packets;
}

void
SelectiveServer::handle_ack(utils::AckPacket &ack_packet) {
    std::cout << "[handle_ack]---Received ack number=" << ack_packet.seqno << '\n';
    window.erase(ack_packet.seqno);
}

void
SelectiveServer::handle_send_state(std::ifstream &input_stream, long chunk_count, long file_size) {
    time_t sent_time = time(nullptr);
    window[next_seq_no] = create_packet(input_stream, chunk_count, file_size);
    send_packet_with_prob(*(window[next_seq_no]));
    pq.push({sent_time, next_seq_no});
    ++next_seq_no;
}

void
SelectiveServer::send_header_packet(long chunk_count) {
    auto header_packet = std::make_unique<utils::Packet>();
    header_packet->seqno = static_cast<uint32_t>(chunk_count);
    header_packet->len = 0;
    send_packet(*header_packet);
}

std::unique_ptr<utils::Packet>
SelectiveServer::create_packet(std::ifstream &input_stream, long chunk_count, long file_size) {
    int send_len = PACKET_LEN;
    if (chunk_count == 1) {
        send_len = static_cast<int>(file_size % PACKET_LEN);
    }
    auto packet = std::make_unique<utils::Packet>();
    input_stream.read(packet->data, send_len);
    packet->len = static_cast<uint16_t>(send_len);
    packet->seqno = next_seq_no;
    return packet;
}

void
SelectiveServer::send_packet(utils::Packet &packet) {
    utils::sendto_wrapper(server_socket, &packet, sizeof(packet) + sizeof(packet.data),
                          (struct sockaddr *) &client_addr, sizeof(client_addr));
    std::cout << "[send_packet]---Send packet successfully" << '\n';
}

void
SelectiveServer::send_packet_with_prob(utils::Packet &packet) {
    if (should_send_packet()) {
        std::cout << "[create_packet]--- Sending packet no=" << packet.seqno << '\n';
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
