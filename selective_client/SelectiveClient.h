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
    SelectiveClient(int server_socket, sockaddr_in &server_addr);
    ~SelectiveClient();

    /** Request a file from the server. */
    void request_file(std::string &filename);
private:
    uint32_t real_base = 0;
    int server_socket;
    struct sockaddr_in server_addr;
    PacketPtr *window = new PacketPtr[WINDOW_SIZE];
    uint32_t recv_base = 0;

    /** Sends ack to the server with the
     *  given ack number. */
    void send_ack(uint32_t ack_no);

    /** Write all the packets that are ready to
     *  be written to the output stream. */
    void packet_clean_up(std::ofstream &output_stream);

    /** Sends the first packet to the server.
     *  in this packet we specify the file name
     *  we are requesting. */
    void send_request_file_packet(std::string &filename);

    /** Receives the header packet from the server.
     *  this packet holds useful information like the
     *  total size of the requested file. */
    std::unique_ptr<utils::Packet> receive_header_packet();
};


#endif //SELECTIVE_CLIENT_SELECTIVECLIENT_H
