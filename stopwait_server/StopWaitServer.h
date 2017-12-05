/**
 *  File: StopWaitServer.h
 *  Description: holder for stop and wait protocol interface
 *  Created at: 2017-12-1
 */

#ifndef ALTERNATING_SERVER_STOPWAITSERVER_H
#define ALTERNATING_SERVER_STOPWAITSERVER_H

#include "socket_utils.h"

#define WINDOW_SIZE 512

class StopWaitServer {
public:
    enum State {
        WAIT_FOR_ACK_ZERO,
        SENDING_PACKET_ZERO,
        WAIT_FOR_ACK_ONE,
        SENDING_PACKET_ONE
    };

    void handle_client();
    void receive_client_request(int server_socket, sockaddr_in &client_addr);
private:

    State handle_wait_for_ack_zero(int server_socket, sockaddr_in &client_addr, utils::Packet *packet);

    void set_connection_time_out(int server_socket);

    void send_packet(int server_socket, sockaddr_in &client_addr, utils::Packet *packet);

    std::pair<State, utils::Packet *> handle_sending_packet_zero(int server_socket, sockaddr_in &client_addr, std::ifstream &input_stream);

    std::pair<State, utils::Packet *> handle_sending_packet_one(int server_socket, sockaddr_in &client_addr, std::ifstream &input_stream);

    State handle_wait_for_ack_one(int server_socket, sockaddr_in &client_addr, utils::Packet *packet_to_send);

    void handle_sending_file(int server_socket, sockaddr_in &client_addr, utils::Packet *packet);
};


#endif //ALTERNATING_SERVER_STOPWAITSERVER_H
