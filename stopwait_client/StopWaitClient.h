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
    void get_file(std::string& requested_file, std::string& destination_path, int server_socket);
    void request_file(std::string &filename, int server_socket, sockaddr_in server_addr);
private:

    void send_ack(int server_socket);
};


#endif //ALTERNATING_CLIENT_ALTERCLIENT_H
