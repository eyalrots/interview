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

class Server
{
  protected:
    int listening_sockfd;
    int conn_sockfd;
    struct sockaddr_in listening_addr;

  public:
    Server();
    ~Server();

    Error_Code open_listening_socket();
    virtual Error_Code listen_and_connect() = 0;
};

class Echo_Server : public Server
{
  public:
    Error_Code listen_and_connect() override;
};
#endif /* SERVER_H */