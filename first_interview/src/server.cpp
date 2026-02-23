#include "../include/server.hpp"

constexpr int PORT = 3490;
constexpr int BACKLOG = 10;

Server::Server()
{
    memset(&this->listening_addr, 0, sizeof(this->listening_addr));

    this->listening_addr =
        (struct sockaddr_in){.sin_family = AF_INET,
                             .sin_port = htons(PORT),
                             .sin_addr = {.s_addr = INADDR_ANY}};

    this->listening_sockfd = 0;
}

Error_Code Tcp_Server::open_listening_socket()
{
    int status;

    this->listening_sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
}

Error_Code Echo_Server::listen_and_connect()
{
    socklen_t sin_size = 0;

    constexpr int len = 256;
    char buffer[len] = {0};

    int bytes_reveived = 0;
    struct sockaddr_storage client_addr;

    memset(&client_addr, 0, sizeof(client_addr));

    /* Listen for incomming connections */
    if (listen(this->listening_sockfd, BACKLOG) == -1) {
        std::cerr << "listen" << std::endl;
        return Error_Code::LISTEN_ERR;
    }

    std::cout << "server: waiting for connections..." << std::endl;

    while (true) {
        
        sin_size = sizeof(client_addr);

        /* Take the first connection in the queue */
        auto conn_fd = Descriptor(accept(this->listening_sockfd,
                                   (struct sockaddr *)&client_addr, &sin_size));
        if (conn_fd.get_fd() == -1) {
            std::cerr << "accept" << std::endl;
            continue;
        }

        /* Receive a message */
        bytes_reveived = recv(conn_fd.get_fd(), buffer, len, 0);
        if (bytes_reveived == -1) {
            std::cerr << "Server: receive" << std::endl;
            continue;
        }

        /* Send the message that was received */
        buffer[bytes_reveived] = '\0';
        if (send(conn_fd.get_fd(), buffer, bytes_reveived, 0) == -1) {
            std::cerr << "Server: send" << std::endl;
            continue;
        }
    }
    return Error_Code::OK;
}

Error_Code Dns_Sniffer::open_listening_socket()
{
    int status;

    this->listening_sockfd =
        socket(this->listening_addr.sin_family, SOCK_RAW, 0);
    if (this->listening_sockfd == -1) {
        std::cerr << "Sever: socket" << std::endl;
        return Error_Code::SOCK_ERR;
    }

    status =
        bind(this->listening_sockfd, (struct sockaddr *)&this->listening_addr,
             sizeof(this->listening_addr));
    if (status == -1) {
        std::cerr << "Server: bind" << std::endl;
        return Error_Code::BIND_ERR; // return ErrorCode
    }
    return Error_Code::OK;
}

Error_Code Dns_Sniffer::listen_and_connect()
{
    socklen_t sin_size = 0;

    constexpr int len = 256;
    char buffer[len] = {0};
    int bytes_received = 0;
    struct sockaddr_storage sniffed_addr;

    memset(&sniffed_addr, 0, sizeof(sniffed_addr));

    // How does a DNS sniffer work?
    
    return Error_Code::OK;
}

Server::~Server()
{
    if (this->listening_sockfd > 0) {
        close(this->listening_sockfd);
	}
}