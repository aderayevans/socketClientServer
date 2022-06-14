#include <iostream>

#include "tcplistener.hpp"


int main(int argc, char const* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Lack of arguments" << std::endl;
        return EXIT_FAILURE;
    }
    TCPListener tcp_server = TCPListener(argv[1], argv[2]);
    
    if (tcp_server.__init())
    {
        tcp_server.__run();
    }

    return EXIT_SUCCESS;
}