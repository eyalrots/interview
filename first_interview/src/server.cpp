#include "../include/server.hpp"
#include <sys/socket.h>

Server::Server()
{
    memset(&listening_addr, 0, sizeof(listening_addr));
    listening_addr.sin_family = AF_INET;
    listening_addr.sin_port = htons(PORT);
    listening_addr.sin_addr.s_addr = INADDR_ANY;

    listening_sockfd = 0;
    conn_sockfd = 0;
}

int Server::open_listening_socket()
{
    int status = 0;

    listening_sockfd = socket(listening_addr.sin_family, SOCK_STREAM, 0);
    if (listening_sockfd == -1) {
        std::cerr << "Sever: socket" << std::endl;
    }

    status = bind(listening_sockfd, (struct sockaddr *)&listening_addr,
                  sizeof(listening_addr));
    if (status == -1) {
        std::cerr << "Server: bind" << std::endl;
        return 1;
    }
    return 0;
}

void Server::listen_and_connect() {}

void Echo_Server::listen_and_connect()
{
    socklen_t sin_size = 0;
    char buffer[256];
    int len = 256;
    int msg_len = 0;
    int bytes_sent = 0;
    struct sockaddr_storage client_addr;

    memset(&client_addr, 0, sizeof(client_addr));
    memset(buffer, 0, sizeof(buffer));

    /* Listen for incomming connections */
    if (listen(listening_sockfd, BACKLOG) == -1) {
        std::cerr << "listen" << std::endl;
    }

    std::cout << "server: waiting for connections..." << std::endl;

    while (true) {
        sin_size = sizeof(client_addr);

        /* Take the first connection in the queue */
        conn_sockfd = accept(listening_sockfd, (struct sockaddr *)&client_addr,
                             &sin_size);
        if (conn_sockfd == -1) {
            std::cerr << "accept" << std::endl;
            continue;
        }

        /* Receive a message */
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
    }
}

Server::~Server()
{
    close(conn_sockfd);
    close(listening_sockfd);
}