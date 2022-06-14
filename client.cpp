#include <iostream>

#include "tcpclient.hpp"


int main(int argc, char const* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Lack of arguments" << std::endl;
        return EXIT_FAILURE;
    }
    TCPClient tcp_client = TCPClient(argv[1], argv[2]);
    
    if (tcp_client.__init())
    {
        tcp_client.__run();
    }

    return EXIT_SUCCESS;
}