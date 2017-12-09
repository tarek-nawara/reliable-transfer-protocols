/**
 *  File: SelectiveServer.h
 *  Description: Interface for the selective repeat protocol
 *  Created at: 2017-12-8
 */

#ifndef SELECTIVE_SERVER_SELECTIVESERVER_H
#define SELECTIVE_SERVER_SELECTIVESERVER_H

#include "SentPacket.h"
#include "socket_utils.h"

#define TIME_OUT_DURATION 2

typedef SentPacket* SentPacketPtr;

class SelectiveServer {
public:
    SelectiveServer(int server_socket, double plp, unsigned int seed);
    ~SelectiveServer();

    void handle_client_request();

private:
    struct sockaddr_in client_addr;
    int server_socket;
    double plp;
    uint32_t window_size = 10;
    SentPacketPtr *window = new SentPacketPtr[window_size];
    uint32_t base = 0;
    uint32_t next_seqno = 0;
    uint32_t sent_count = 0;

    void send_packet(utils::Packet *packet);

    void send_packet_with_prob(utils::Packet *packet);

    bool should_send_packet();

    void handle_sending_file(utils::Packet *packet);

    void handle_ack(utils::AckPacket *ack_packet);

    uint32_t packet_clean_up();

    void send_header_packet(long chunk_count, int window_size);

    utils::Packet *create_packet(std::ifstream &input_stream, long chunk_count, long file_size);

    void handle_send_state(std::ifstream &input_stream, long chunk_count, long file_size);
};


#endif //SELECTIVE_SERVER_SELECTIVESERVER_H
