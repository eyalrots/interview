#include "../include/server.hpp"
#include <string>

constexpr int PORT = 3490;
constexpr int BACKLOG = 10;
constexpr int UDP_NUM = 17;
constexpr int DNS_PORT = 53;

My_packet::My_packet(struct iphdr* iph, struct udphdr* udph, char* data)
{
    this->iph = iph;
    this->udph = udph;
    this->data = data;
}

Error_Code process_packet(char *buffer, My_packet * &p)
{
    struct iphdr *iph;
    struct udphdr *udph;
    char *data;

    int iph_size = 0;

    p = NULL;

    iph = (struct iphdr *)buffer;
    if (iph->protocol != UDP_NUM) {
        return Error_Code::OK;
    }
    iph_size = iph->ihl * 4;
    udph = (struct udphdr *)(buffer + iph_size);
    if (ntohs(udph->dest) != DNS_PORT) {
        return Error_Code::OK;
    }
    data = buffer + iph_size + sizeof(*udph);

    p = new My_packet(iph, udph, data);

    return Error_Code::OK;
}

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
    this->listening_sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (this->listening_sockfd == -1) {
        std::cerr << "DNS Sniffer: socket" << std::endl;
        return Error_Code::SOCK_ERR;
    }
    
    return Error_Code::OK;
}

Error_Code Dns_Sniffer::listen_and_connect()
{
    socklen_t sin_size = 0;
    constexpr int len = 2048; /* 2K sized buffer */
    char buffer[len] = {0};
    int bytes_received = 0;
    struct sockaddr_storage sniffed_addr;
    My_packet *cur_packet = NULL;

    memset(&sniffed_addr, 0, sizeof(sniffed_addr));

    // How does a DNS sniffer work?
    sin_size = sizeof(sniffed_addr);
	
    while (true) {
        bytes_received = recvfrom(this->listening_sockfd, buffer, len, 0,
                                  (struct sockaddr *)&sniffed_addr, &sin_size);
        if (bytes_received == -1) {
            std::cerr << "DNS Sniffer: receive" << std::endl;
            continue;
        }
        process_packet(buffer, cur_packet);
        if (cur_packet == NULL) {
            continue;
		}
        std::cout << "Domain: " << cur_packet->get_data() << std::endl;
        delete cur_packet;
	}
    
    return Error_Code::OK;
}

Server::~Server()
{
    if (this->listening_sockfd > 0) {
        close(this->listening_sockfd);
	}
}