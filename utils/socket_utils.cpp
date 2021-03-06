/**
 * File: socket_utils.cpp
 * Description: implementation for socket wrapper functions
 * Created Date: 2017-11-15
 */

#include "socket_utils.h"

namespace utils {
    int socket_wrapper() {
        int server_socket;
        if ((server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
            utils::die_with_error("failed");
        }
        return server_socket;
    }

    void connect_wrapper(int socket, const struct sockaddr *foreign_addr, socklen_t addr_len) {
        if (connect(socket, foreign_addr, addr_len) < 0) {
            die_with_error("connect() failed");
        }
    }

    ssize_t send_wrapper(int socket,
                         const void *msg,
                         size_t msg_len,
                         int flags) {
        if (send(socket, msg, msg_len, flags) != msg_len) {
            die_with_error("send() sent different number of bytes than expected");
        }
        return msg_len;
    }

    ssize_t recv_wrapper(int socket,
                         void *rcv_buffer,
                         size_t buffer_len,
                         int flags) {
        ssize_t total_rcv = recv(socket, rcv_buffer, buffer_len, flags);
        if (total_rcv < 0) {
            die_with_error("recv() failed or connection closed  prematurely");
        }
        return total_rcv;
    }

    void bind_wrapper(int socket, const struct sockaddr *local_addr, socklen_t addr_len) {
        if (bind(socket, local_addr, addr_len) < 0) {
            die_with_error("bind() failed");
        }
    }

    void listen_wrapper(int socket, int queue_limit) {
        if (listen(socket, queue_limit) < 0) {
            die_with_error("listen() failed");
        }
    }

    void sendto_wrapper(int socket, const void *packet, size_t packet_len, sockaddr *server_addr,
                        socklen_t server_addr_len) {
        ssize_t send_res = sendto(socket, packet, packet_len, 0, server_addr, server_addr_len);
        if (send_res < 0) {
            die_with_error("sendto() failed");
        }
    }


    int accept_wrapper(int socket, struct sockaddr *client_addr, socklen_t *addr_len) {
        int client_socket = accept(socket, client_addr, addr_len);
        if (client_socket < 0) {
            die_with_error("accept() failed");
        }
        return client_socket;
    }

    int inet_pton_wrapper(int family, const char *strptr, struct in_addr *addrptr) {
        if (inet_pton(family, strptr, addrptr) != 1) {
            die_with_error("inet_pton() failed");
        }
        return 1;
    }

    const char *inet_ntop_wrapper(int family, const void *addrptr, char *strptr, socklen_t len) {
        if (inet_ntop(family, addrptr, strptr, len) == NULL) {
            die_with_error("inet_ntop error");
        }
        return strptr;
    }

    ssize_t read_wrapper(int filedes, void *buf, size_t nbytes) {
        ssize_t n = read(filedes, buf, nbytes);
        if (n < 0) {
            die_with_error("read error");
        }

        return n;
    }

    ssize_t write_wrapper(int filedes, const void *buf, size_t nbytes) {
        if (write(filedes, buf, nbytes) != nbytes) {
            die_with_error("write() error");
        }

        return nbytes;
    }

    void close_wrapper(int socket) {
        if (close(socket) < 0) {
            die_with_error("close() failed");
        }
    }

    long file_size(const std::string &filename) {
        struct stat stat_buf;
        int rc = stat(filename.c_str(), &stat_buf);
        return rc == 0 ? stat_buf.st_size : -1;
    }

    void append_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv) {
        std::ofstream outfile;
        outfile.open(file_name, std::ios_base::app);
        outfile.write(rcv_buf, byte_rcv);
        outfile.close();
    }

    void write_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv) {
        std::ofstream outfile(file_name);
        outfile.write(rcv_buf, byte_rcv);
        outfile.close();
    }

    pid_t fork_wrapper() {
        pid_t process_id;
        if ((process_id = fork()) < 0) {
            die_with_error("fork() failed");
        }
        return process_id;
    }

    pid_t waitpid_wrapper() {
        pid_t process_id = waitpid((pid_t) -1, NULL, WNOHANG);
        if (process_id < 0) {
            die_with_error("waitpid() failed");
        }
        return process_id;
    }

    std::string &left_trim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    std::string &right_trim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    std::string &trim(std::string &s) {
        return left_trim(right_trim(s));
    }

    std::unique_ptr<std::vector<std::string>> read_parameters(std::string input_file_name) {
        std::ifstream input_file(input_file_name);
        std::string ip_addr, filename;
        std::string line;
        auto res = std::make_unique<std::vector<std::string>>();
        while (getline(input_file, line)) {
            line = utils::trim(line);
            if (line.empty()) continue;
            res->push_back(line);
        }
        return res;
    }

    void set_connection_time_out(int server_socket, int seconds) {
        struct timeval tv = {seconds, 0};
        if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            perror("Error");
        }
    }

    void init_server_addr(sockaddr_in &server_addr, std::uint32_t ip_addr, uint16_t port_number) {
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(ip_addr);
        server_addr.sin_port = htons(port_number);
    }

    void write_packet(std::ofstream &output_stream, Packet *packet) {
        for (int i = 0; i < packet->len; ++i) {
            output_stream << packet->data[i];
        }
    }

    void die_with_error(const char *error_msg) {
        perror(error_msg);
        exit(1);
    }
}

