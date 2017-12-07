//
// Created by tarek on 12/7/17.
//

#include <socket_utils.h>
#include "SelectiveClient.h"

SelectiveClient::SelectiveClient(int server_socket, sockaddr_in &server_addr) {
    this->server_socket = server_socket;
    this->server_addr = server_addr;
}

void
SelectiveClient::request_file(std::string &filename) {
    socklen_t server_addr_size = sizeof(server_addr);

    auto *packet = send_request_file_packet(filename);

    std::ofstream output_stream;
    output_stream.open(filename);

    auto chunk_count = packet->len;
    this->window_size = packet->seqno;
    this->window = new PacketPtr[this->window_size];

    while (chunk_count > 0) {
        auto *data_packet = new utils::Packet();
        ssize_t recv_res = recvfrom(server_socket, data_packet, sizeof(*data_packet), 0,
                                    (struct sockaddr *) &server_addr, &server_addr_size);
        if (recv_res > 0) { // OUT of window
            send_ack(data_packet->seqno);
            window[data_packet->seqno] = packet;
            --chunk_count;
        }
        packet_clean_up(output_stream);
    }
    output_stream << std::flush;
}

utils::Packet *
SelectiveClient::send_request_file_packet(std::string &filename) {
    auto *packet = new utils::Packet();
    packet->seqno = 0;
    packet->len = filename.length();
    for (size_t i = 0; i < filename.length(); ++i) {
        packet->data[i] = filename[i];
    }
    sendto_wrapper(server_socket, packet, sizeof(packet) + sizeof(packet->data),
                   (struct sockaddr *) &server_addr, sizeof(server_addr));
    std::cout << "[request_file]---Send packet successfully" << '\n';
    return packet;
}

void
SelectiveClient::packet_clean_up(std::ofstream &output_stream) {
    while (window[rec_base] != nullptr) {
        write_packet(output_stream, window[rec_base]);
        rec_base = (rec_base + 1) % window_size;
    }
}

void
SelectiveClient::send_ack(uint32_t ack_no) {
    auto *ack_packet = new utils::AckPacket();
    ack_packet->seqno = ack_no;
    utils::sendto_wrapper(server_socket, ack_packet, sizeof(ack_packet),
                          (struct sockaddr *) &server_addr, sizeof(server_addr));
    std::cout << "[send_ack]---Send ack packet with ackno=" << ack_no << '\n';
}

void
SelectiveClient::write_packet(std::ofstream &output_stream, utils::Packet *packet) {
    for (int i = 0; i < packet->len; ++i) {
        output_stream << packet->data[i];
    }
}
