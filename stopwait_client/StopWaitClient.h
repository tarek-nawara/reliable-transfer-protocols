/**
 * File: AlterClient.h
 * Description: Alternating client interface
 * Created at: 2017-12-1
 */

#ifndef ALTERNATING_CLIENT_ALTERCLIENT_H
#define ALTERNATING_CLIENT_ALTERCLIENT_H

#include "socket_utils.h"

class StopWaitClient {
public:
    enum class State {
        WAIT_FOR_PACKET_ZERO,
        WAIT_FOR_PACKET_ONE,
        TERMINATE
    };

    StopWaitClient(int server_socket, sockaddr_in &server_addr);

    void request_file(std::string &filename);

private:
    int server_socket;
    sockaddr_in server_addr;

    void send_ack(uint32_t ack_no);

    State handle_wait_for_packet_zero(std::ofstream &output_stream);

    State handle_wait_for_packet_one(std::ofstream &output_stream);

    State handle_wait_for_packet(uint32_t seqno, State next_state, std::ofstream &output_stream);
};


#endif //ALTERNATING_CLIENT_ALTERCLIENT_H
