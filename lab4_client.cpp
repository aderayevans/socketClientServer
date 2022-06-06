#include <iostream>

#include "tcpclient.hpp"


int main(int argc, char const* argv[])
{
    TCPClient tcp_client = TCPClient(argv[1], argv[2]);
    
    if (tcp_client.__init())
    {
        tcp_client.__run();
    }

    return EXIT_SUCCESS;
}