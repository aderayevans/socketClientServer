#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <fstream>
#include <sstream>

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

#include "iohandler.hpp"

const int SOCKETERROR = -1;

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
    void __send_int(uint32_t &, int);
    bool __testing();

    void __read_file_chunk();
    bool __write_file_chunk();
    bool __uploadfile(std::string);
    bool __downloadfile(std::string);

private:
    const char* raw_port, * raw_ip_addr;
    std::string __header = "GET / HTTP/1.1\r\n\r\n";
    sockaddr_in __remote_address;
    int __socket;
    // char __recv_buf[BUFFSIZE] = {'\0'};
    std::string __recv_buf;
    
    std::string __send_buf;
    IOHandler *__ioHandler = new IOHandler();
};

#endif