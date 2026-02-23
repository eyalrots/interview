#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <net/ethernet.h>
#include <linux/if_packet.h>

enum class Error_Code { OK = 0, SOCK_ERR, BIND_ERR, LISTEN_ERR };

class Descriptor
{
  private:
    int fd;

  public:
    Descriptor(int fd)
    {
        this->fd = fd;
    }
    ~Descriptor()
    {
        close(this->fd);
    }
    int get_fd()
    {
        return this->fd;
    }
};

class My_packet
{
  private:
    struct iphdr* iph;
    struct udphdr* udph;
    char *data;

  public:
    My_packet(struct iphdr* iph, struct udphdr* udph, char* data);
    ~My_packet();

    struct iphdr* ip_header()
    {
        return this->iph;
    }
    struct udphdr* udp_header()
    {
        return this->udph;
    }
    char* get_data()
    {
        return this->data;
    }
};

class Server
{
  protected:
    int listening_sockfd;
    struct sockaddr_in listening_addr;

  public:
    Server();
    ~Server();

    virtual Error_Code open_listening_socket() = 0;
    virtual Error_Code listen_and_connect() = 0;
};

class Tcp_Server : public Server
{
  public:
    Error_Code open_listening_socket() override;
};

class Echo_Server : public Tcp_Server
{
  public:
    Error_Code listen_and_connect() override;
};

class Dns_Sniffer : public Server
{
  public:
    Error_Code open_listening_socket() override;
    Error_Code listen_and_connect() override;
};

#endif /* SERVER_H */