/** File: main.cpp
 *  Description: entry point for the selective server
 *  Created at: 2017-12-1
 */

#include <iostream>
#include "socket_utils.h"

void set_connection_time_out(int server_socket);

int main() {
	std::cout << "hello world" << '\n';
	return 0;
}

void set_connection_time_out(int server_socket) {
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		perror("Error");
	}
}