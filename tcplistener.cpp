#include "tcplistener.hpp"


TCPListener::TCPListener()
    : raw_ip_addr("127.0.0.1"), raw_port("8080")
{
}

TCPListener::TCPListener(const char* ip_addr, const char* port)
    : raw_ip_addr(ip_addr), raw_port(port)
{
}

bool TCPListener::__init()
{
    // create a socket
    if (!__create_socket()) return false;
    // bind the socket to a ip / port
    if (!__bind()) return false;
    // mark the socket for listening
    if (!__listen()) return false;
    // accept the call
    if (!__accept()) return false;
    // close the listening socket
    close(__socket);

    // send welcome message
    __send(__header.data(), __header.size());

    return true;
}

void TCPListener::__run()
{
    // send and receive
    while (true)
    {
        // clear the buffer
        memset(__buf, 0, 4096);
        // wait for the message
        int bytesRecv = __recv();

        if (bytesRecv == NetworkConnectStatus::Failed)
        {
            std::cerr << "There was a connection issue!" << std::endl;
            break;
        }
        if (bytesRecv == 0)
        {
            std::cout << "The client disconnected" << std::endl;
            break;
        }

        std::string response = std::string(__buf, 0, bytesRecv);

        // display message
        std::cout << "Received: " << std::string(__buf, 0, bytesRecv);
        if (response.compare("Close connection") == 0) break; 

        // resend message
        __send(__buf, bytesRecv + 1);
    }
    // close the socket
    close(__client_socket);
}

bool TCPListener::__create_socket()
{
    if ((this->__socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) 
            == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't create a socket" << std::endl;
		return false;
    }
    return true;
}

void TCPListener::__define_address()
{
    std::string IP_ADDR = raw_ip_addr;
    int PORT = atoi(raw_port);
    // int PORT = 8080;
    __address.sin_family = AF_INET;
    __address.sin_port = htons(PORT);
    // server_address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, IP_ADDR.c_str(), &__address.sin_addr.s_addr);
}

bool TCPListener::__bind()
{
    __define_address();
    
    if (bind(__socket, reinterpret_cast<sockaddr *>(&__address), sizeof(__address))
        == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't bind to IP/PORT" << std::endl;
		return false;
    }
    return true;
}

bool TCPListener::__listen()
{
    if (listen(__socket, SOMAXCONN) == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't listen!" << std::endl;
		return false;
    }
    return true;
}

bool TCPListener::__accept()
{
    socklen_t clientSize = sizeof(__client_address);
    
    __client_socket = accept(__socket, 
                            reinterpret_cast<sockaddr *>(&__client_address), 
                            &clientSize);

    if (__client_socket == NetworkConnectStatus::Failed)
    {
        std::cerr << "Problem with client connecting!" << std::endl;
        return false;
    }

    __print_clien_info();
    return true;
}

void TCPListener::__print_clien_info()
{
    char host[NI_MAXHOST] = {'\0'};
    char service[NI_MAXSERV] = {'\0'};

    int result = getnameinfo(reinterpret_cast<sockaddr *>(&__client_address),
                                sizeof(__client_address),
                                host,
                                NI_MAXHOST,
                                service,
                                NI_MAXSERV,
                                0
                                );

    if (result == NetworkConnectStatus::Succeeded)
    {
        std::cout << host << " connected on " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &__client_address.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(__client_address.sin_port) << std::endl;
    }

}

int TCPListener::__recv()
{
    return recv(__client_socket, &__buf, sizeof(__buf), 0);
}

void TCPListener::__send(char *msg, int len)
{
    send(__client_socket, msg, len, 0);
}