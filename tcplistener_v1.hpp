#ifndef TCPLISTENER_HPP
#define TCPLISTENER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <unordered_map>

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
enum class RESPONSE_STATUS
{
    quit,
    disconnect,
    blank,
    key_notfound,
    key_existed,
    normal
};

class TCPListener
{
public:
    TCPListener();
    TCPListener(const char*, const char*);
    bool __init();
    bool __create_socket();
    bool __bind();
    bool __listen();
    void __define_address();
    bool __accept();
    int __recv();
    void __send(char*, int);
    void __print_clien_info();

    bool __testing();
    void __run();
    std::vector<std::string> get_argv(std::string, char);
    RESPONSE_STATUS __processing(std::string);
    bool __get(std::string);
    bool __put(std::string, std::string);
    bool __del(std::string);

private:
    const char* raw_port, * raw_ip_addr;
    std::string __header = "HTTP/1.1 200 OK\r\n\n";
    // char server_message[1024] = "You have reached the server!\r\n";
    sockaddr_in __address, __client_address;
    int __socket, __client_socket;
    char __buf[4096] = {'\0'};
    std::unordered_map<std::string, std::string> __database;
};

#endif