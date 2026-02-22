#include "../include/server.h"
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>

Server::Server()
{
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; /* IPv4 */
    hints.ai_socktype = SOCK_STREAM; /* TCP connection */
    hints.ai_flags = AI_PASSIVE; /* Use my IP */

    listening_sockfd = 0;
    conn_sockfd = 0;
}

int Server::open_listening_socket()
{
    int status = 0;
    int yes = 1;

    /* Get information for socket */
    status = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return 1;
    }

    /* Loop through potential sockets untill you find a working one */
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Try to open a socket in this location - current p */
        listening_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listening_sockfd == -1) {
            std::cerr << "Sever: socket" << std::endl;
            continue;
        }

        /* Use same one as before with no problems */
        status = setsockopt(listening_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                            sizeof(int));
        if (status == -1) {
            std::cerr << "setsockopt" << std::endl;
            exit(1);
        }

        /* Bind to selected socket */
        status = bind(listening_sockfd, p->ai_addr, p->ai_addrlen);
        if (status == -1) {
            std::cerr << "Server: bind" << std::endl;
            continue;
        }

        /* Found a socket and bounded to it */
        break;
    }
    
	/* No sockets where found */
    if (p == NULL) {
        std::cerr << "Server: failed to bind" << std::endl;
        exit(1);
    }

    return 0;
}

void Server::listen_and_connect()
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
        if (send(conn_sockfd, buffer, len, 0) == -1) {
            std::cerr << "Server: send" << std::endl;
            continue;
        }
    }
}

Server::~Server()
{
    freeaddrinfo(servinfo);
    freeaddrinfo(p);
    close(conn_sockfd);
    close(listening_sockfd);
}