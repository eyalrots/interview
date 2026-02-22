// fix include
// should ne
// #include "server.hpp"
#include "../include/server.hpp"
#include <sys/socket.h>

// macro alternative
// constexpr int PORT = 3490;
// constexpr int BACKLOG = 10;

// somewhere in this file
// you have a leak, find it
Server::Server()
{
    // use this keyword
    // memset(&this->listening_addr, 0, sizeof(this->listening_addr));

    // designated initializer alternative
    // this->listening_addr = (struct sockaddr_in){
    //     .sin_family = AF_INET,
    //     .sin_port = htons(PORT),
    //     .sin_addr = {.s_addr = INADDR_ANY},
    // };

    // this->listening_sockfd = 0;
    // this->conn_sockfd = 0;

    memset(&listening_addr, 0, sizeof(listening_addr));
    listening_addr.sin_family = AF_INET;
    listening_addr.sin_port = htons(PORT);
    listening_addr.sin_addr.s_addr = INADDR_ANY;

    listening_sockfd = 0;
    conn_sockfd = 0;
}

// im only using ErrorCode here, but use it everywhere
int Server::open_listening_socket()
// ErrorCode Server::open_listening_socket()
{
    // can delete this
    int status = 0;

    listening_sockfd = socket(listening_addr.sin_family, SOCK_STREAM, 0);
    if (listening_sockfd == -1) {
        std::cerr << "Sever: socket" << std::endl;
        return 1;
        // return ErrorCode::SOCK_ERR;
    }

    status = bind(listening_sockfd, (struct sockaddr *)&listening_addr,
                  sizeof(listening_addr));
    if (status == -1) {
        std::cerr << "Server: bind" << std::endl;
        return 1;
        // return ErrorCode::BIND_ERR;
    }

    // return ErrorCode::OK;
    return 0;
}

// can remove this, look at declaration in header file
void Server::listen_and_connect()
{
}

void Echo_Server::listen_and_connect() // add override keyword
{
    socklen_t sin_size = 0;

    // constexpr int len = 256;
    // char buffer[len] = {0}; // zero initialize the buffer
    char buffer[256];
    int len = 256;

    // no need for this
    int msg_len = 0;
    int bytes_sent = 0;
    struct sockaddr_storage client_addr;

    memset(&client_addr, 0, sizeof(client_addr));
    memset(buffer, 0, sizeof(buffer)); // can remove

    /* Listen for incomming connections */
    if (listen(listening_sockfd, BACKLOG) == -1) {
        std::cerr << "listen" << std::endl;
        // return error code here
    }

    std::cout << "server: waiting for connections..." << std::endl;

    while (true) {
        sin_size = sizeof(client_addr);

        /* Take the first connection in the queue */

        // dont forget this keyword
        // this->conn_sockfd = accept(this->listening_sockfd, (struct sockaddr
        // *)&client_addr,
        //                      &sin_size);
        conn_sockfd = accept(listening_sockfd, (struct sockaddr *)&client_addr,
                             &sin_size);
        if (conn_sockfd == -1) {
            std::cerr << "accept" << std::endl;
            continue;
        }

        /* Receive a message */
        // this->
        // recv returns the number of bytes received, use it instead of strlen,
        // faster and safer
        // int bytes_received = recv(this->conn_sockfd, buffer, len, 0);
        // if (bytes_received == -1) {
        //     std::cerr << "Server: receive" << std::endl;
        //     continue;
        // }

        // buffer[bytes_received] = '\0'; // null terminate the buffer

        // if (send(this->conn_sockfd, buffer, bytes_received, 0) == -1) {
        //     std::cerr << "Server: send" << std::endl;
        //     continue;
        // }

        if (recv(conn_sockfd, buffer, len, 0) == -1) {
            std::cerr << "Server: receive" << std::endl;
            continue;
        }

        /* Send the message that was received */
        msg_len = strlen(buffer);
        if (send(conn_sockfd, buffer, msg_len, 0) == -1) {
            std::cerr << "Server: send" << std::endl;
            continue;
        }

        // Close the connection socket
        // you have a leak here, conn_sockfd will be overwritten in the next
        // loop
    }
}

Server::~Server()
{
    close(conn_sockfd);
    close(listening_sockfd);
    // if (listening_sockfd != 0) {
    //     close(listening_sockfd);
    // }
    // if (conn_sockfd != 0) {
    //     close(conn_sockfd);
    // }
    // you can also use shutdown here, but close is fine for this use case
    // shutdown(conn_sockfd, SHUT_RDWR);

    // dont forget to zero out the file descriptors, to avoid double close in case of multiple
}