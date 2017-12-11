/**
 *  File: SelectiveServer.h
 *  Description: Interface for the selective repeat protocol
 *  Created at: 2017-12-8
 */

#ifndef SELECTIVE_SERVER_SELECTIVESERVER_H
#define SELECTIVE_SERVER_SELECTIVESERVER_H

#include "socket_utils.h"


#define TIME_OUT_DURATION 2

typedef std::pair<time_t, uint32_t> SentStatus;

class SelectiveServer {
public:
    /** Constructor. */
    SelectiveServer(int server_socket, double plp, unsigned int seed, uint32_t max_window_size);

    /** Handle a single client request. */
    void handle_client_request();

private:
    class SentStatusComparator {
    public:
        bool operator()(SentStatus status_one, SentStatus status_two) {
            return status_one.first > status_two.first;
        }
    };

    struct sockaddr_in client_addr;
    int server_socket;
    double plp;
    uint32_t window_size = 100;
    std::map<uint32_t, std::unique_ptr<utils::Packet>> window;

    std::priority_queue<SentStatus, std::vector<SentStatus>, SentStatusComparator> pq;

    uint32_t next_seq_no = 0;
    uint32_t max_window_size = 0;

    /** Test wither we should send this specific
     *  packet or not. */
    bool should_send_packet();

    /** This method will resend all the packets that
     *  timed out. */
    uint32_t packet_clean_up();

    /** Read from the input stream and builds a packet. */
    std::unique_ptr<utils::Packet> create_packet(std::ifstream &input_stream, long chunk_count, long file_size);

    /** This method will handle sending a packet to the client and
     *  all the updates in the current state. */
    void handle_send_state(std::ifstream &input_stream, long chunk_count, long file_size);

    /** Send a single packet to the client */
    void send_packet(utils::Packet &packet);

    /** Send a single packet to the client with a probability that
     *  the packet may not be sent. */
    void send_packet_with_prob(utils::Packet &packet);

    /** Handle receiving the ack process. */
    void handle_ack(utils::AckPacket &ack_packet);

    /** Send an entire file to the client. */
    void handle_sending_file(utils::Packet &request_packet);

    /** Sends a header packet to the client, this packet
     *  contains useful information like the total size
     *  of the requested file. */
    void send_header_packet(long chunk_count);
};


#endif //SELECTIVE_SERVER_SELECTIVESERVER_H
