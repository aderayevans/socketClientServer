#include <iostream>

#include "tcplistener_v1.hpp"


int main(int argc, char const* argv[])
{
    TCPListener tcp_server = TCPListener(argv[1], argv[2]);
    
    if (tcp_server.__init())
    {
        tcp_server.__run();
    }

    return EXIT_SUCCESS;
}