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



const int SOCKETERROR = -1;

enum class RESPONSE_STATUS
{
    quit,
    blank,
    key_notfound,
    key_existed,
    normal
};

const int BUFFER_SIZE = 1024;
const int MAX_CLIENTS = 100;

class TCPListener
{
public:
    TCPListener();
    TCPListener(const char*, const char*);
    bool __init();
    bool __setup_server();
    bool __create_socket();
    bool __bind();
    bool __listen();
    void __define_address();
    void __run();
    bool handle_connection(int);
    int __accept(sockaddr_in &);
    int __recv(int);
    void __send(int, char*, int);
    void __print_client_info(int, sockaddr_in);
    bool __check(int, const char *);

    bool __testing(int);
    std::vector<std::string> get_argv(std::string, char);
    RESPONSE_STATUS __processing(int, std::string);
    bool __get(int, std::string);
    bool __put(std::string, std::string);
    bool __del(std::string);

private:
    const char* raw_port, * raw_ip_addr;
    const int server_backlog = SOMAXCONN;
    std::string __header = "HTTP/1.1 200 OK\r\n\n";
    // char server_message[1024] = "You have reached the server!\r\n";
    sockaddr_in __address;
    int __socket;
    char __buf[BUFFER_SIZE] = {'\0'};
    std::unordered_map<std::string, std::string> __database;
    timeval timeout;

};

#endif