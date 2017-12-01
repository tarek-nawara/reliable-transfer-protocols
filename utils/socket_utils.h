/**
 * File: socket_utils.h
 * Description: socket wrapper methods
 * Created Date: 2017-11-15
 */

#ifndef SOCKET_PROGRAMMING_SOCKET_UTILS_H
#define SOCKET_PROGRAMMING_SOCKET_UTILS_H

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <netdb.h>
#include <netinet/in.h>
#include <cstdarg>
#include <arpa/inet.h>
#include <strings.h>
#include <string>
#include <iostream>
#include <cstring>
#include <ostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <cstdlib>
#include <utility>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAXLINE 4096
#define MEDIUMLINE 256
#define MINILINE 16
#define MAXPINDING 5

namespace utils {

    /**
     * Creating a socket.
     *
     * @return         -1 for failure, or socket number for success
     */
    int socket_wrapper();

    /**
     * Establish connection with the given address.
     *
     * @param socket         socket number.
     * @param foreignAddress pointer to sockaddr containing internet address and port of the server
     * @param address_len    length of the address structure = {@code sizeof(struct sockaddr_in)}
     */
    void connect_wrapper(int socket, const struct sockaddr *foreign_addr, socklen_t addr_len);

    /**
     * Send a message to the given address.
     *
     * @param socket       socket number
     * @param msg          pointer on the message to send
     * @param msg_len      length of the message to send
     * @param flags        way to change the default behavior
     * @param foreign_addr pointer to sockaddr containing internet address and port of the server
     * @param addr_len     length of the address structure = {@code sizeof(struct sockaddr_in)}
     * @return             -1 for failure, length of sent message for success
     */
    ssize_t send_wrapper(int socket,
                         const void *msg,
                         size_t msg_len,
                         int flags);

    /**
     * Receive message from the given address.
     *
     * @param socket       socket number
     * @param rcv_buffer   pointer on rcv buffer
     * @param buffer_len   length of the rcv buffer
     * @param flags        way to change the default behavior
     * @param foreign_addr pointer to sockaddr containing internet address and port of the server
     * @param addr_len     length of the address structure = {@code sizeof(struct sockaddr_in)}
     * @return             -1 for failure, length of sent message for success
     */
    ssize_t recv_wrapper(int socket,
                        void *rcv_buffer,
                        size_t buffer_len,
                        int flags);

    /**
     * Assign to the socket a local address and a port.
     *
     * @param socket     socket number
     * @param local_addr struct containing the internet address and port to listen on
     * @param addr_len   length of the address struct = {@code sizeof(struct sockaddr_in}
     */
    void bind_wrapper(int socket, const struct sockaddr *local_addr, socklen_t addr_len);

    /**
     * Causes internal state changes to the given socket.
     *
     * @param socket      socket number
     * @param queue_limit limit of incoming connections that can be waiting at any time
     */
    void listen_wrapper(int socket, int queue_limit);

    /**
     * Dequeue the next connection on the queue of the socket,
     * if the queue is empty it will block until a connection request
     * arrives.
     *
     * @param socket      socket number
     * @param client_addr address of the client
     * @param addr_len    maximum size of client address
     * @return            descriptor for a new socket that is connected to the client if
     *                    successful, -1 on failure
     */
    int accept_wrapper(int socket, struct sockaddr *client_addr, socklen_t *addr_len);

    /**
     * Close the connection.
     *
     * @param socket socket number
     */
    void close_wrapper(int socket);

    /* Read the ip address of the given struct address */
    int inet_pton_wrapper(int family, const char *strptr, struct in_addr *addrptr);

    /* Read the port number of the given struct address*/
    const char *inet_ntop_wrapper(int family, const void *addrptr, char *strptr, socklen_t len);

    /* Read data from the given socket */
    ssize_t read_wrapper(int filedes, void *buf, size_t nbytes);

    /* Write data on the given socket */
    ssize_t write_wrapper(int filedes, const void *buf, size_t nbytes);

    /* Get the file size of the given file path */
    long file_size(const std::string &filename);

    /* Append the given buffer to a file */
    void append_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv);

    /* Create a new file then write the given buffer to this file */
    void write_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv);

    /* Fork a new process, if fork fails will terminate
       Program execution */
    pid_t fork_wrapper();

    /* Wait for one of the child processes */
    pid_t waitpid_wrapper();

    /* Trim all the whitespaces from left of the string */
    std::string &left_trim(std::string &s);

    /* Trim all the whitespaces from right of the string */
    std::string &right_trim(std::string &s);

    /* Trim all whitespaces from both sides of a string */
    std::string &trim(std::string &s);

    /**
     * Log an error message and terminate program execution.
     *
     * @param error_msg error message to log
     */
    void die_with_error(const char *error_msg);
}

#endif //SOCKET_PROGRAMMING_SOCKET_UTILS_H
