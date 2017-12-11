/**
 *  File: StopWaitServer.h
 *  Description: holder for stop and wait protocol interface
 *  Created at: 2017-12-1
 */

#ifndef ALTERNATING_SERVER_STOPWAITSERVER_H
#define ALTERNATING_SERVER_STOPWAITSERVER_H

#include "socket_utils.h"

#define TIME_OUT_DURATION 2

class StopWaitServer {
public:
    StopWaitServer(int server_socket, double plp, unsigned int seed);

    enum class State {
        WAIT_FOR_ACK_ZERO,
        SENDING_PACKET_ZERO,
        WAIT_FOR_ACK_ONE,
        SENDING_PACKET_ONE
    };

    /** Handle a single client request. */
    void handle_client_request();

private:
    struct sockaddr_in client_addr;
    int server_socket;
    double plp;
    long chunk_count = 0;
    long file_size = 0;

    /** Handle sending an entire file to the client. */
    void handle_sending_file(utils::Packet &request_packet);

    /** Handle waiting for ack zero state. */
    State handle_wait_for_ack_zero(utils::Packet &packet_to_send);

    /** Handle sending the first packet state. */
    std::pair<State, std::shared_ptr<utils::Packet>> handle_sending_packet_zero(std::ifstream &input_stream);

    /** Handle waiting for the second ack state. */
    State handle_wait_for_ack_one(utils::Packet &packet_to_send);

    /** Handle sending second packet state. */
    std::pair<State, std::shared_ptr<utils::Packet>> handle_sending_packet_one(std::ifstream &input_stream);

    /** Generic method for handling any wait for ack state. */
    State handle_wait_for_ack(utils::Packet &packet_to_send, uint32_t ack_no, State next_state);

    /** Test wither we should send the current
     *  packet to the client or not.*/
    bool should_send_packet();

    /** Sending a packet to the client, this may or
     *  may not send the packet depending on the given
     *  probability. */
    void send_packet_with_prob(utils::Packet &packet);

    /** Generic method for handling any sending packet state. */
    std::pair<State, std::shared_ptr<utils::Packet>>
    handle_sending_packet(std::ifstream &input_stream, uint32_t packet_no, State next_state);

    /** Send a single packet to the client. */
    void send_packet(utils::Packet &packet);
};


#endif //ALTERNATING_SERVER_STOPWAITSERVER_H
