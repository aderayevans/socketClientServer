#ifndef TCPLISTENER_HPP
#define TCPLISTENER_HPP

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

class TCPListener
{
public:
    TCPListener();
    TCPListener(const char*, const char*);
    bool __init();
    void __run();
    bool __create_socket();
    bool __bind();
    bool __listen();
    void __define_address();
    bool __accept();
    int __recv();
    void __send(char*, int);
    void __print_clien_info();

private:
    const char* raw_port, * raw_ip_addr;
    std::string __header = "HTTP/1.0 200 OK\r\n\n";
    // char server_message[1024] = "You have reached the server!\r\n";
    sockaddr_in __address, __client_address;
    int __socket, __client_socket;
    char __buf[4096] = {'\0'};
};

#endif