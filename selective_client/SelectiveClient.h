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
    SelectiveClient(int server_socket, sockaddr_in &server_addr);

    void request_file(std::string &filename);
private:
    int window_size = 10;
    int server_socket;
    struct sockaddr_in server_addr;
    PacketPtr *window;
    uint32_t rec_base = 0;

    void send_ack(uint32_t ack_no);

    void write_packet(std::ofstream &output_stream, utils::Packet *packet);

    void packet_clean_up(std::ofstream &output_stream);

    utils::Packet *send_request_file_packet(std::string &filename);
};


#endif //SELECTIVE_CLIENT_SELECTIVECLIENT_H
