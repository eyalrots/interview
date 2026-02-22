#include "../include/server.hpp"

int main()
{
    Echo_Server *s = new Echo_Server();
    s->open_listening_socket();
    s->listen_and_connect();
    delete s;
    return 0;
}