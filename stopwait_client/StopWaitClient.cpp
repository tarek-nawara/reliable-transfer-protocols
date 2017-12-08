/**
 * File: StopWaitClient.h
 * Description: interface for the stop and wait protocol.
 * Created at: 2017-12-1
 */

#include <socket_utils.h>
#include "StopWaitClient.h"

StopWaitClient::StopWaitClient(int server_socket, sockaddr_in &server_addr) {
    this->server_socket = server_socket;
    this->server_addr = server_addr;
}

void
StopWaitClient::request_file(std::string &filename) {
    auto *packet = new utils::Packet();
    packet->seqno = 0;
    packet->len = filename.length();
    for (size_t i = 0; i < filename.length(); ++i) {
        packet->data[i] = filename[i];
    }
    utils::sendto_wrapper(server_socket, packet, sizeof(packet) + sizeof(packet->data),
                          (struct sockaddr *) &server_addr, sizeof(server_addr));
    std::cout << "[request_file]---Send packet successfully" << '\n';
    std::ofstream output_stream;
    output_stream.open(filename);
    auto current_state = StopWaitClient::State::WAIT_FOR_PACKET_ZERO;
    while (true) {
        switch (current_state) {
            case StopWaitClient::State::WAIT_FOR_PACKET_ZERO:
                current_state = handle_wait_for_packet_zero(output_stream);
                break;
            case StopWaitClient::State::WAIT_FOR_PACKET_ONE:
                current_state = handle_wait_for_packet_one(output_stream);
                break;
            case StopWaitClient::State::TERMINATE:
                output_stream << std::flush;
                output_stream.close();
                return;
        }
    }
}

StopWaitClient::State
StopWaitClient::handle_wait_for_packet_zero(std::ofstream &output_stream) {
    return handle_wait_for_packet(0, StopWaitClient::State::WAIT_FOR_PACKET_ONE, output_stream);
}

StopWaitClient::State
StopWaitClient::handle_wait_for_packet_one(std::ofstream &output_stream) {
    return handle_wait_for_packet(1, StopWaitClient::State::WAIT_FOR_PACKET_ZERO, output_stream);
}

StopWaitClient::State
StopWaitClient::handle_wait_for_packet(uint32_t seqno, StopWaitClient::State next_state, std::ofstream &output_stream) {
    auto *packet = new utils::Packet();
    while (true) {
        socklen_t server_addr_size = sizeof(server_addr);
        ssize_t recv_res = recvfrom(server_socket, packet, sizeof(*packet), 0,
                                    (struct sockaddr *) &server_addr, &server_addr_size);
        if (recv_res > 0 && packet->seqno == seqno) {
            utils::write_packet(output_stream, packet);
            send_ack(seqno);
            return next_state;
        } else if (recv_res > 0 && packet->len == 0) {
            return StopWaitClient::State::TERMINATE;
        } else {
            send_ack((seqno + 1) % 2);
        }
    }
}

void
StopWaitClient::send_ack(uint32_t ack_no) {
    auto *ack_packet = new utils::AckPacket();
    ack_packet->seqno = ack_no;
    utils::sendto_wrapper(server_socket, ack_packet, sizeof(ack_packet),
                          (struct sockaddr *) &server_addr, sizeof(server_addr));
    std::cout << "[send_ack]---Send ack packet with ackno=" << ack_no << '\n';
}

