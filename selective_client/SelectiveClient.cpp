/**
 *  File: SelectiveClient.cpp
 *  Description: Implementation for the selective repeat receiver
 *  Created at: 2017-12-8
 */

#include <socket_utils.h>
#include "SelectiveClient.h"

SelectiveClient::SelectiveClient(int server_socket, sockaddr_in &server_addr) {
    this->server_socket = server_socket;
    this->server_addr = server_addr;
}

SelectiveClient::~SelectiveClient() {
    for (size_t i = 0; i < WINDOW_SIZE; ++i) {
        delete window[i];
    }
    delete[] window;
}

void
SelectiveClient::request_file(std::string &filename) {
    socklen_t server_addr_size = sizeof(server_addr);

    send_request_file_packet(filename);

    std::ofstream output_stream;
    output_stream.open(filename);

    auto header_packet = receive_header_packet();
    auto chunk_count = header_packet->seqno;

    std::fill(window, window + WINDOW_SIZE, nullptr);

    while (chunk_count > 0) {
        auto *data_packet = new utils::Packet();
        ssize_t recv_code = recvfrom(server_socket, data_packet, sizeof(*data_packet), 0,
                                     (struct sockaddr *) &server_addr, &server_addr_size);
        if (recv_code > 0) {
            if (data_packet->seqno < expected_seq_no) {
                send_ack(data_packet->seqno);
            } else if (data_packet->seqno >= expected_seq_no && data_packet->seqno < (expected_seq_no + WINDOW_SIZE)) {
                send_ack(data_packet->seqno);
                window[(data_packet->seqno) % WINDOW_SIZE] = data_packet;
                std::cout << "[request_file]---Receiving new packet with seqno=" << data_packet->seqno << '\n';
                --chunk_count;
            }
        }

        std::cout << "[request_file]---Chunk count=" << chunk_count << '\n';
        packet_clean_up(output_stream);
    }
    output_stream << std::flush;
}

void
SelectiveClient::send_request_file_packet(std::string &filename) {
    auto packet = std::make_unique<utils::Packet>();
    packet->seqno = 0;
    packet->len = static_cast<uint16_t>(filename.length());
    filename.copy(packet->data, filename.length(), 0);
    sendto_wrapper(server_socket, packet.get(), sizeof(*packet) + sizeof(packet->data),
                   (struct sockaddr *) &server_addr, sizeof(server_addr));
    std::cout << "[send_request_file_packet]---Send request packet successfully" << '\n';
}

void
SelectiveClient::packet_clean_up(std::ofstream &output_stream) {
    std::cout << "[packet_clean_up]" << '\n';
    while (window[recv_base] != nullptr) {
        utils::write_packet(output_stream, window[recv_base]);
        delete window[recv_base];
        window[recv_base] = nullptr;
        recv_base = (recv_base + 1) % WINDOW_SIZE;
        ++expected_seq_no;
    }
}

void
SelectiveClient::send_ack(uint32_t ack_no) {
    auto ack_packet = std::make_unique<utils::AckPacket>();
    ack_packet->seqno = ack_no;
    utils::sendto_wrapper(server_socket, ack_packet.get(), sizeof(*(ack_packet)),
                          (struct sockaddr *) &server_addr, sizeof(server_addr));
    std::cout << "[send_ack]---Send ack packet with ackno=" << ack_no << '\n';
}

std::unique_ptr<utils::Packet>
SelectiveClient::receive_header_packet() {
    auto header_packet = std::make_unique<utils::Packet>();
    socklen_t server_addr_size = sizeof(server_addr);
    while (true) {
        ssize_t recv_res = recvfrom(server_socket, header_packet.get(), sizeof(*header_packet), 0,
                                    (struct sockaddr *) &server_addr, &server_addr_size);
        if (recv_res > 0) {
            return header_packet;
        }
    }
}