/**
 *  File: SelectiveClient.h
 *  Description: Interface for selective repeat receiver
 *  Created at: 2017-12-8
 */

#ifndef SELECTIVE_CLIENT_SELECTIVECLIENT_H
#define SELECTIVE_CLIENT_SELECTIVECLIENT_H

#include "socket_utils.h"

typedef utils::Packet* PacketPtr;

class SelectiveClient {
public:
    SelectiveClient(int server_socket, sockaddr_in &server_addr, double plp, unsigned int seed);

    void request_file(std::string &filename);
private:
    uint32_t window_size = 10;
    int server_socket;
    struct sockaddr_in server_addr;
    PacketPtr *window;
    uint32_t recv_base = 0;
    double plp;

    void send_ack(uint32_t ack_no);

    void write_packet(std::ofstream &output_stream, utils::Packet *packet);

    void packet_clean_up(std::ofstream &output_stream);

    void send_request_file_packet(std::string &filename);

    utils::Packet *receive_header_packet();

    bool should_send_packet();

    void send_ack_with_prob(uint32_t ack_no);
};


#endif //SELECTIVE_CLIENT_SELECTIVECLIENT_H
