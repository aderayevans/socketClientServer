#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <iostream>
#include <string>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
//define NI_MAXHOST NI_MAXSERV
#include <netdb.h>
//struct store address information
#include <netinet/in.h>
// closing the connected socket
#include <unistd.h>
// for inet_pton, convert the ip addr format
#include <arpa/inet.h>

enum NetworkConnectStatus {
    Succeeded = 0,
    Failed = -1
};

class TCPClient
{
public:
    TCPClient();
    TCPClient(const char*, const char*);
    bool __init();
    void __run();
    bool __create_socket();
    void __define_address();
    bool __connect();
    int __recv();
    void __send(char*, int);
    bool __testing();

private:
    const char* raw_port, * raw_ip_addr;
    std::string __header = "GET / HTTP/1.1\r\n\r\n";
    sockaddr_in __remote_address;
    int __socket;
    char __recv_buf[4096] = {'\0'};
    std::string __send_buf;
};

#endif