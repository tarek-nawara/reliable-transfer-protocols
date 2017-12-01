//
// Created by tarek on 12/1/17.
//

#ifndef ALTERNATING_SERVER_STOPWAITSERVER_H
#define ALTERNATING_SERVER_STOPWAITSERVER_H

#include "socket_utils.h"

class StopWaitServer {
public:
    void handle_client();
    void receive_client_request(int server_socket, sockaddr_in &client_addr);
private:

};


#endif //ALTERNATING_SERVER_STOPWAITSERVER_H
