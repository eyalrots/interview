#include "../include/server.hpp"
using namespace std;


int main()
{
    // option 1: stack allocation
    Echo_Server server{};
    server.open_listening_socket();
    server.listen_and_connect();


    // option 2: heap allocation, using smart pointer
    unique_ptr<Echo_Server> server2 = make_unique<Echo_Server>();
    server2->open_listening_socket();
    server2->listen_and_connect();

    // Echo_Server *s = new Echo_Server();
    // s->open_listening_socket();
    // s->listen_and_connect();
    // delete s;

    return 0;
}