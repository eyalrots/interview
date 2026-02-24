#include "../include/server.hpp"
#include <linux/if_ether.h>
#include <string>

constexpr int PORT = 3490;
constexpr int BACKLOG = 10;
constexpr int UDP_NUM = 17;
constexpr int DNS_PORT = 53;

My_packet::My_packet(char* packet)
{
    this->raw = packet;
    this->l3_offset = 0;
    this->l4_offset = 0;
    this->l5_offset = 0;

    size_t l3_size = 0;
    size_t l4_size = 0;

    /* Get layer two header (ethernet) */
    auto eth_hdr = reinterpret_cast<struct ethhdr *>(packet);
    if (eth_hdr->h_proto != ETH_P_IP) {
        return; /* We are only intrested in IPv4 */
    }
	l3_size = sizeof(struct ethhdr);
    /* Get layer three header (IP) */
    auto ip_hdr = reinterpret_cast<struct iphdr *>(packet + l3_size);
    if (ip_hdr->protocol != UDP_NUM) {
        return; /* We are only intrested in UDP (for DNS) */
    }
    l4_size = l3_size + sizeof(struct iphdr);
    /* Get layer four (UDP) */
    auto udp_hdr = reinterpret_cast<struct udphdr *>(packet + l4_size);
    if (udp_hdr->dest != DNS_PORT) {
        return; /* We are only intrested in port 53 (for DNS) */
    }

    this->l3_offset = l3_size;
    this->l4_offset = l4_size;
    this->l5_offset = l4_size + sizeof(struct udphdr);
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
        auto cur_packet = My_packet(buffer);
        
        std::cout << "Domain: " << cur_packet.dns_data() << std::endl;
	}
    
    return Error_Code::OK;
}

Server::~Server()
{
    if (this->listening_sockfd > 0) {
        close(this->listening_sockfd);
	}
}