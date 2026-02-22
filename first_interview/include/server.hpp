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

// these can be moved to .cpp file
// no need to expose these to the user of the class
#define PORT 3490
#define BACKLOG 10

// do better error handling, using enum class for error codes
enum class Error_Code {
    OK = 0,
    SOCK_ERR,
    BIND_ERR,
};

class Server
{
  protected:
    int listening_sockfd;
    int conn_sockfd;
    struct sockaddr_in listening_addr;

  public:
    Server();
    ~Server();

    int open_listening_socket();

    // use zero initializer for this, for pure virtual function
    // virtual void listen_and_connect() = 0;
    virtual void listen_and_connect();
};

class Echo_Server : public Server
{
  public:
    void listen_and_connect();
};
#endif /* SERVER_H */