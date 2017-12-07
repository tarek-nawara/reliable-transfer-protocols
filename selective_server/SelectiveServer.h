//
// Created by tarek on 12/7/17.
//

#ifndef SELECTIVE_SERVER_SELECTIVESERVER_H
#define SELECTIVE_SERVER_SELECTIVESERVER_H

#include "SentPacket.h"
#include "socket_utils.h"

#define TIME_OUT_DURATION 2
#define WINDOW_SIZE 10

typedef SentPacket* SentPacketPtr;

class SelectiveServer {
public:
    SelectiveServer(int server_socket, double plp, unsigned int seed);

    void handle_client_request();

private:
    struct sockaddr_in client_addr;
    int server_socket;
    double plp;
    SentPacketPtr *window = new SentPacketPtr[WINDOW_SIZE];
    uint32_t base = 0;
    uint32_t next_seqno = 0;
    int sent_count = 0;

    void send_packet(utils::Packet *packet);

    void send_packet_with_prob(utils::Packet *packet);

    bool should_send_packet();

    void handle_sending_file(utils::Packet *packet);

    utils::Packet *create_packet(std::ifstream &input_stream);

    long handle_send_state(std::ifstream &input_stream, long chunk_count);

    void handle_ack(utils::AckPacket *packet);

    void packet_clean_up();
};


#endif //SELECTIVE_SERVER_SELECTIVESERVER_H
