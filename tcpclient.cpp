#include "tcpclient.hpp"


TCPClient::TCPClient()
    : raw_ip_addr("127.0.0.1"), raw_port("8080")
{
}

TCPClient::TCPClient(const char* ip_addr, const char* port)
    : raw_ip_addr(ip_addr), raw_port(port)
{
}

bool TCPClient::__init()
{
    // create a socket
    if (!__create_socket()) return false;

    // connect to a server
    if (!__connect()) return false;

    // send welcome message
    __send(__header.data(), __header.size());

    return true;
}

void TCPClient::__run()
{
    // send and receive
    while (true)
    {
        // clear the buffer
        __send_buf.clear();
        memset(__recv_buf, 0, 4096);
        // memset(send_buf, 0, 4096);

        // wait for the message
        int bytesRecv = __recv();

        if (bytesRecv == NetworkConnectStatus::Failed)
        {
            std::cerr << "There was a connection issue!" << std::endl;
            break;
        }
        if (bytesRecv == 0)
        {
            std::cout << "The server disconnected" << std::endl;
            break;
        }
        std::cout << "Received: " << std::string(__recv_buf, 0, bytesRecv);

        // wait for input
        // std::cin >> send_buf;
		std::getline(std::cin, __send_buf);
        __send_buf += "\r\n";
        if (__send_buf.compare("QUIT\r\n") == 0) 
        {
            __send(std::string("Close connection\r\n").data(), 19);
            break;
        }
        __send(__send_buf.data(), __send_buf.size());
        // std::cout << "Sending " << send_buf;
    }
    // close the socket
    close(__socket);
}

bool TCPClient::__create_socket()
{
    if ((this->__socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) 
            == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't create a socket" << std::endl;
		return false;
    }
    return true;
}

void TCPClient::__define_address()
{
    std::string IP_ADDR = raw_ip_addr;
    int PORT = atoi(raw_port);
    // int PORT = 8080;
    __remote_address.sin_family = AF_INET;
    __remote_address.sin_port = htons(PORT);
    // server_address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, IP_ADDR.c_str(), &__remote_address.sin_addr.s_addr);
}

bool TCPClient::__connect()
{
    __define_address();

    if (connect(__socket, reinterpret_cast<struct sockaddr *>(&__remote_address), sizeof(__remote_address))
        == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't connect to IP/PORT" << std::endl;
		return false;
    }
    return true;
}

int TCPClient::__recv()
{
    return recv(__socket, &__recv_buf, sizeof(__recv_buf), 0);
}

void TCPClient::__send(char *msg, int len)
{
    send(__socket, msg, len, 0);
}