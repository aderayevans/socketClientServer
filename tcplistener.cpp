#include "tcplistener.hpp"
#include <sstream>
#include <algorithm>
#include <iterator>
#include "menuchoice.hpp"


TCPListener::TCPListener()
    : raw_ip_addr("127.0.0.1"), raw_port("8080")
{
}

TCPListener::TCPListener(const char* ip_addr, const char* port)
    : raw_ip_addr(ip_addr), raw_port(port)
{
}

bool TCPListener::__setup_server()
{
    // create a socket
    if (!__create_socket()) return false;
    // bind the socket to a ip / port
    if (!__bind()) return false;
    // mark the socket for listening
    if (!__listen()) return false;
    return true;
}

bool TCPListener::__init()
{
    if (!__setup_server()) return false;

    return true;
}

bool TCPListener::__create_socket()
{
    // creating a socket
    /*
    __domain:   AF_INET -> ipv4 only
                AF_INET6 -> ipv6 and ipv4
                AF_APPLETALK -> apple computer
                AF_UNIX -> unix file system

    __type: SOCK_STREAM -> using TCP to communicate
            SOCK_DGRAM ->  using UDP to communicate
            SOCK_RAW -> provides access to ICMP
            ...

    __protocol: 
        IPPROTO_IP = 0,
        IPPROTO_ICMP = 1,
        IPPROTO_TCP = 6,
        IPPROTO_UDP = 17,
        IPPROTO_IPV6 = 41,
        IPPROTO_RAW = 255,
        ...

    */
    return __check(this->__socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP), 
                    "Can't create a socket");
}

void TCPListener::__define_address()
{
    std::string IP_ADDR = raw_ip_addr;
    int PORT = atoi(raw_port);
    // int PORT = 8080;
    __address.sin_family = AF_INET;
    __address.sin_port = htons(PORT);
    // INADDR_ANY = 0.0.0.0
    // server_address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, IP_ADDR.c_str(), &__address.sin_addr.s_addr);
}

bool TCPListener::__bind()
{
    __define_address();
    
    return __check(bind(__socket, reinterpret_cast<sockaddr *>(&__address), sizeof(__address)),
                "Can't bind to IP/PORT");
}

bool TCPListener::__check(int exp, const char *msg)
{
    if (exp == SOCKETERROR)
    {
        // perror(msg);
        std::cerr << msg << std::endl;
        return false;
        // exit(EXIT_FAILURE);
    }
    return true;
}

bool TCPListener::__listen()
{
    return __check(listen(__socket, server_backlog),
                "Can't listen!");
}

int TCPListener::__accept(sockaddr_in & __client_address)
{
    socklen_t clientSize = sizeof(__client_address);

    // __client_socket = accept(__socket, NULL, NULL);
    int __client_socket = accept(__socket, 
                            reinterpret_cast<sockaddr *>(&__client_address), 
                            &clientSize);
    return __client_socket;
}

void TCPListener::__print_client_info(ClientPacket &__client_packet, sockaddr_in __client_address)
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

    if (result == 0)
    {
        std::cout << host << " connected on " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &__client_address.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(__client_address.sin_port) << std::endl;
    }
    __client_packet.host = host;
    __client_packet.service = service;
}

bool TCPListener::__testing(int __client_socket)
{
    // memset(__buf, 0, BUFFER_SIZE);
    __buf.clear();
    __buf.resize(BUFFER_SIZE);
    int bytesRecv = __recv(__client_socket);
    // __buf.resize(bytesRecv);
    // std::string response = std::string(__buf, 0, bytesRecv);
    std::string response(__buf.begin(), __buf.begin() + bytesRecv);

    __check(bytesRecv, "There was a connection issue!");

    if (bytesRecv == 0)
    {
        std::cout << "The client disconnected" << std::endl;
        return false;
    }
    std::cout << response;
    // send welcome message
    __send(__client_socket, __header.data(), __header.size());
    return true;
}

int TCPListener::__recv(int __client_socket)
{
    // return recv(__client_socket, &__buf, sizeof(__buf), 0);
    return recv(__client_socket, __buf.data(), __buf.size(), 0);
}

void TCPListener::__send(int __client_socket, char *msg, int len)
{
    send(__client_socket, msg, len, 0);
}

bool TCPListener::handle_connection(ClientPacket __client_packet)
{
    // send and receive
    // while (true)
    {
        __buf.clear();
        __buf.resize(BUFFER_SIZE);

        // clear the buffer
        // memset(__buf, 0, BUFFER_SIZE);
        int bytesRecv = __recv(__client_packet.__client_socket);
        // __buf.resize(bytesRecv);

        if (!__check(bytesRecv, "There was a connection issue!")) return false;

        if (bytesRecv == 0)
        {
            std::cout << "The client disconnected" << std::endl;
            return false;
        }

        // std::string response = std::string(__buf, 0, bytesRecv);
        std::string response(__buf.begin(), __buf.begin() + bytesRecv);

        RESPONSE_STATUS response_status = __processing(__client_packet, response);

        if (response_status == RESPONSE_STATUS::quit)
        {
            __send(__client_packet.__client_socket, std::string("Closing connection, bye\r\n").data(), 26);
            return false;
        }
        else if (response_status == RESPONSE_STATUS::blank)
        {
            __send(__client_packet.__client_socket, std::string("Syntax error\r\n").data(), 15);
        }
        else if (response_status == RESPONSE_STATUS::key_notfound)
        {
            __send(__client_packet.__client_socket, std::string("KEY NOTFOUND\r\n").data(), 15);
        }
        else if (response_status == RESPONSE_STATUS::key_existed)
        {
            __send(__client_packet.__client_socket, std::string("KEY EXISTED\r\n").data(), 14);
        }
        // else if (response_status == RESPONSE_STATUS::normal)
        else
        {
            __send(__client_packet.__client_socket, std::string("Command succeeded\r\n").data(), 20);
        }
    }
    return true;
    // close(__client_socket);
}

bool FD_IS_ANY_SET(fd_set const *fdset)
{
    static fd_set empty;     // initialized to 0 -> empty
    return memcmp(fdset, &empty, sizeof(fd_set)) != 0;
}

void TCPListener::__run()
{
    // std::vector<int> __client_sockets(MAX_CLIENTS, 0);

    std::array<ClientPacket, MAX_CLIENTS> __client_packets;
    ClientPacket __tmp_packet;

    for (auto &val:__client_packets)
        val.__client_socket = 0;

    int STD_INPUT = 1;
    fd_set read_sockets;
    FD_SET(STD_INPUT, &read_sockets);

    int max_socket_so_far, __tmp_socket, __new_socket;

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    std::cout << "Listening, waiting for connections . . .\n";

    // max_socket_so_far = ([](int x, int y) -> int
    // {
    //     return x > y ? x:y;
    // }, __socket, __udp_socket) + 1;

    while (!__close_server_bit)
    {
        // initialize
        FD_ZERO(&read_sockets);
        FD_SET(__socket, &read_sockets);
        // timeout.tv_sec = 0;
        // timeout.tv_usec = 50;
        max_socket_so_far = __socket;

        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            __tmp_socket = __client_packets[i].__client_socket;

            if (__tmp_socket > 0)
            {
                std::cout << "[FD_SET]: " << __tmp_socket << std::endl;
                FD_SET(__tmp_socket, &read_sockets);
            }

            if (__tmp_socket > max_socket_so_far)
                max_socket_so_far = __tmp_socket;
        }

        // selecting
        if (select(max_socket_so_far + 1, &read_sockets, NULL, NULL, NULL) < 0)
        {
            std::cerr << "[Select error]" << std::endl;
            // break;
            exit(EXIT_FAILURE);
        }
        std::cout << "[max_socket_so_far]: " << max_socket_so_far << std::endl;
        
        // handling socket from main
        if (FD_ISSET(__socket, &read_sockets))
        {
            // this is a new connection
            // accept the call
            sockaddr_in __client_address;
            __new_socket = __accept(__client_address);
            std::cout << "[Accepted]: " << __new_socket << std::endl;
            if (!__check(__new_socket, "Problem with client connecting!"))
                continue;
            ClientPacket __new_packet;
            __new_packet.__client_socket = __new_socket;
            __print_client_info(__new_packet, __client_address);

            // try to communicate to test the network
            if(!__testing(__new_socket)) return;

            for (int i = 0; i < MAX_CLIENTS; ++i)
            {
                if (__client_packets[i].__client_socket == 0)
                {
                    __client_packets[i] = __new_packet;
                    std::cout << "[Adding socket]: " << __new_socket << std::endl;
                    break;
                }
            }
        }

        // std::string __getline;
        // std::getline(std::cin, __getline);
        // if (__getline == "QUIT")
        //     close(__socket);

        // handling socket from clients
        for (int i = 0; i < max_socket_so_far; ++i)
        {
            __tmp_packet = __client_packets[i];

            if (FD_ISSET(__tmp_packet.__client_socket, &read_sockets))
            {
                std::cout << "[Handling]: " << __tmp_packet.__client_socket << std::endl;
                if (!handle_connection(__tmp_packet))
                {
                    std::cout << "[Disconnecting host]: " << __tmp_packet.__client_socket << std::endl;
                    close(__tmp_packet.__client_socket);
                    __client_packets[i].__client_socket = 0;
                }
            }
            else
            {
                std::cout << "[Not Handling]: " << __tmp_packet.__client_socket << std::endl;
            }
        }
    }
    std::cout << "[SERVER CLOSING]" << std::endl;
    close(__socket);
}

RESPONSE_STATUS TCPListener::__processing(ClientPacket __client_packet, std::string response)
{
    std::cout << "Received: \"" << response << "\"";
    std::cout << "Size of response = " << response.size() << std::endl;

    response.pop_back();
    response.pop_back();

    std::vector<std::string> __argv = get_argv(response, ' ');
    int __argc = __argv.size();
    // std::cout << "Number of arguments = " << __argc << std::endl;

    // check argv
    if (__argc < 1)
    {
        return RESPONSE_STATUS::blank;
    }


    switch(ChoiceClass::hashTheChoice(__argv[0]))
    {
        case menuChoice::quit:
            return RESPONSE_STATUS::quit;
        case menuChoice::help:
            __send(__client_packet.__client_socket, ChoiceClass::getMenu().data(), ChoiceClass::getMenu().size());
            return RESPONSE_STATUS::normal;
        case menuChoice::get:
            if (__argc != 2)
                return RESPONSE_STATUS::blank;
            if (!__get(__client_packet.__client_socket, __argv[1]))
                return RESPONSE_STATUS::key_notfound;
            break;
        case menuChoice::put:
            if (__argc != 3)
                return RESPONSE_STATUS::blank;
            if (!__put(__client_packet, __argv[1], __argv[2]))
                return RESPONSE_STATUS::key_existed;
            break;
        case menuChoice::del:
            if (__argc != 2)
                return RESPONSE_STATUS::blank;
            if (!__del(__client_packet, __argv[1]))
                return RESPONSE_STATUS::key_notfound;
            break;
        case menuChoice::getfile:
            if (__argc != 2)
                return RESPONSE_STATUS::blank;
            if (!__getfile(__client_packet.__client_socket, __argv[1]))
                return RESPONSE_STATUS::file_notfound;
            break;
        case menuChoice::putfile:
            if (__argc != 2)
                return RESPONSE_STATUS::blank;
            if (!__putfile(__client_packet, __argv[1]))
                return RESPONSE_STATUS::file_uploadfailed;
            break;
        case menuChoice::delfile:
            if (__argc != 2)
                return RESPONSE_STATUS::blank;
            if (!__delfile(__client_packet, __argv[1]))
                return RESPONSE_STATUS::file_notfound;
            break;
        case menuChoice::secretcommand:
            if (__argc != 4)
                return RESPONSE_STATUS::blank;
            if (!__secretcommand(__client_packet, __argv[1], __argv[2],  __argv[3]))
                return RESPONSE_STATUS::blank;
            break;
        case menuChoice::wrong:
            return RESPONSE_STATUS::blank;
            break;
    }
        
    return RESPONSE_STATUS::normal;
}

std::vector<std::string> TCPListener::get_argv(std::string big_string, char split)
{
    std::string temp_string;
    std::vector<std::string> __vector;
    std::stringstream ss(big_string);
    while (std::getline(ss, temp_string, split))
        __vector.push_back(temp_string);
    
    // for (auto &each:__vector)
    //     for (std::string::iterator it = each.begin(); it != each.end(); it++)
    //         if (!isgraph((*it)))
    //             each.erase(it + 1);
    return __vector;
}

bool TCPListener::__secretcommand(ClientPacket __client_packet, std::string user, std::string passwd, std::string secretcommand)
{
    /*
        COMMAND LIST:
            CLOSE: close server
    */
    std::unordered_map<std::string, std::string> __listOfAdmin;
    
    __listOfAdmin = {
        {"admin", "socketserver$2022"},
        };

    if (__listOfAdmin.count(user) <= 0) return false;
    for (auto __admin:__listOfAdmin)
    {
        if (__admin.first == user && __admin.second != passwd)
        {
            return false;
        }
    }

    if (SecretChoice::hashTheChoice(secretcommand) == SecretChoice::SECRETCOMMAND::CLOSE)
    {
        std::cout << "Closing signal received" << std::endl;
        __send(__client_packet.__client_socket, std::string("Sending close signal to server\r\n").data(), 33);
        __close_server_bit = true;
    }

    return true;
}

bool TCPListener::__get(int __client_socket, std::string key)
{
    if (__database.empty()) return false;
    if (__database.count(key) == 0) return false;

    __send(__client_socket, (__database[key].second + "\r\n").data(), __database[key].second.size() + 2);
    return true;
}

bool TCPListener::__put(ClientPacket __client_packet, std::string key, std::string val)
{
    if (__database.count(key) > 0) return false;
    std::pair<ClientPacket, std::string> __pair;
    __pair.first = __client_packet;
    __pair.second = val;
    __database[key] = __pair;
    return true;
}

bool TCPListener::__del(ClientPacket __client_packet, std::string key)
{
    if (__database.empty()) return false;
    for (auto author:__database)
    {
        if (author.first == key)
        {
            if (author.second.first.host != __client_packet.host)
            {
                __send(__client_packet.__client_socket, std::string("You mustn't delete other data!!!\r\n").data(), 35);
                return false;
            }
            if (author.second.first.service != __client_packet.service)
            {
                __send(__client_packet.__client_socket, std::string("You mustn't delete other data!!!\r\n").data(), 35);
                return false;
            }
            break;
        }
    }

    return __database.erase(key);
}

/*



                                FILE STUFF



*/


bool TCPListener::__getfile(int __client_socket, std::string filename)
{
    // if (__database.empty()) return false;
    // if (__database.count(filename) == 0) return false;

    // __send(__client_socket, (__database[key].second + "\r\n").data(), __database[key].second.size() + 2);
    return true;
}

bool TCPListener::__putfile(ClientPacket __client_packet, std::string filename)
{
    // recv filename
    while (true)
    {
        std::cout << filename << std::endl;
    } 

    // if (__database.count(key) > 0) return false;
    // std::pair<ClientPacket, std::string> __pair;
    // __pair.first = __client_packet;
    // __pair.second = val;
    // __database[key] = __pair;
    return true;
}

bool TCPListener::__delfile(ClientPacket __client_packet, std::string filename)
{
    // if (__database.empty()) return false;
    // for (auto author:__database)
    // {
    //     if (author.first == key)
    //     {
    //         if (author.second.first.host != __client_packet.host)
    //         {
    //             __send(__client_packet.__client_socket, std::string("You mustn't delete other data!!!\r\n").data(), 35);
    //             return false;
    //         }
    //         if (author.second.first.service != __client_packet.service)
    //         {
    //             __send(__client_packet.__client_socket, std::string("You mustn't delete other data!!!\r\n").data(), 35);
    //             return false;
    //         }
    //         break;
    //     }
    // }

    return true;
}