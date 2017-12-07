/**
 *  File: StopWaitServer.h
 *  Description: holder for stop and wait protocol interface
 *  Created at: 2017-12-1
 */

#ifndef ALTERNATING_SERVER_STOPWAITSERVER_H
#define ALTERNATING_SERVER_STOPWAITSERVER_H

#include "socket_utils.h"

#define TIMEOUT 2

class StopWaitServer {
public:
    enum class State {
        WAIT_FOR_ACK_ZERO,
        SENDING_PACKET_ZERO,
        WAIT_FOR_ACK_ONE,
        SENDING_PACKET_ONE
    };

    explicit StopWaitServer(int server_socket);

    void handle_client();

    void receive_client_request();

private:
    struct sockaddr_in client_addr;
    int server_socket;

    void handle_sending_file(utils::Packet *request_packet);

    State handle_wait_for_ack_zero(utils::Packet *packet_to_send);

    std::pair<State, utils::Packet *> handle_sending_packet_zero(std::ifstream &input_stream);

    State handle_wait_for_ack_one(utils::Packet *packet_to_send);

    std::pair<State, utils::Packet *> handle_sending_packet_one(std::ifstream &input_stream);

    void send_packet(utils::Packet *packet);

    std::pair<State, utils::Packet *>
    handle_sending_packet(std::ifstream &input_stream, uint32_t packet_no, State next_state);

    State handle_wait_for_ack(utils::Packet *packet_to_send, uint32_t ack_no, State next_state);
};


#endif //ALTERNATING_SERVER_STOPWAITSERVER_H
