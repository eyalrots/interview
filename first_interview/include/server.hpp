#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#define PORT 3490
#define BACKLOG 10

class Server
{
  private:
    int listening_sockfd;
    int conn_sockfd;
    struct sockaddr_in listening_addr;
  public:
    Server();
    ~Server();

    int open_listening_socket();
    void listen_and_connect();
};
#endif /* SERVER_H */