/**
 *  File: SelectiveClient.h
 *  Description: Interface for selective repeat receiver
 *  Created at: 2017-12-8
 */

#ifndef SELECTIVE_CLIENT_SELECTIVECLIENT_H
#define SELECTIVE_CLIENT_SELECTIVECLIENT_H

#include "socket_utils.h"
#define WINDOW_SIZE 100

typedef utils::Packet* PacketPtr;

class SelectiveClient {
public:
    SelectiveClient(int server_socket, sockaddr_in &server_addr, double plp, unsigned int seed);
    ~SelectiveClient();

    void request_file(std::string &filename);
private:
    uint32_t real_base = 0;
    int server_socket;
    struct sockaddr_in server_addr;
    PacketPtr *window = new PacketPtr[WINDOW_SIZE];
    uint32_t recv_base = 0;
    double plp;

    void send_ack(uint32_t ack_no);

    void packet_clean_up(std::ofstream &output_stream);

    void send_request_file_packet(std::string &filename);

    std::unique_ptr<utils::Packet> receive_header_packet();
};


#endif //SELECTIVE_CLIENT_SELECTIVECLIENT_H
