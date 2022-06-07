#include "tcplistener_v1.hpp"
#include <sstream>
#include "menuchoice.hpp"


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

    // try to communicate to test the network
    if(!__testing()) return false;

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
    // INADDR_ANY = 0.0.0.0
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
    
    // __client_socket = accept(__socket, NULL, NULL);
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

bool TCPListener::__testing()
{
    memset(__buf, 0, 4096);
    int bytesRecv = __recv();
    std::string response = std::string(__buf, 0, bytesRecv);

    if (bytesRecv == NetworkConnectStatus::Failed)
    {
        std::cerr << "There was a connection issue!" << std::endl;
        return false;
    }
    if (bytesRecv == 0)
    {
        std::cout << "The client disconnected" << std::endl;
        return false;
    }
    std::cout << response;
    // send welcome message
    __send(__header.data(), __header.size());
    return true;
}

int TCPListener::__recv()
{
    return recv(__client_socket, &__buf, sizeof(__buf), 0);
}

void TCPListener::__send(char *msg, int len)
{
    send(__client_socket, msg, len, 0);
}

void TCPListener::__run()
{
    // send and receive
    while (true)
    {
        // clear the buffer
        memset(__buf, 0, 4096);
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

        RESPONSE_STATUS response_status = __processing(response);

        if (response_status == RESPONSE_STATUS::quit)
        {
            __send(std::string("Closing connection, bye\r\n").data(), 26);
            // break;
        }
        else if (response_status == RESPONSE_STATUS::blank)
        {
            __send(std::string("Syntax error\r\n").data(), 15);
        }
        else if (response_status == RESPONSE_STATUS::key_notfound)
        {
            __send(std::string("KEY NOTFOUND\r\n").data(), 15);
        }
        else if (response_status == RESPONSE_STATUS::key_existed)
        {
            __send(std::string("KEY EXISTED\r\n").data(), 14);
        }
        else if (response_status == RESPONSE_STATUS::normal)
        {
            __send(std::string("Command succeeded\r\n").data(), 20);
        }

        // display message
        // if (response.compare("Close connection") == 0) break;

        // resend message
        // __send(__buf, bytesRecv + 1);
    }
    // close the socket
    close(__client_socket);

}

RESPONSE_STATUS TCPListener::__processing(std::string response)
{
    // std::cout << "Received: \"" << response << "\"";
    // std::cout << "Size of response = " << response.size() << std::endl;
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


    switch(hashTheChoice(__argv[0]))
    {
        case menuChoice::quit:
            return RESPONSE_STATUS::quit;
        case menuChoice::help:
            __send(getMenu().data(), getMenu().size());
            return RESPONSE_STATUS::normal;
        case menuChoice::get:
            if (__argc != 2)
                return RESPONSE_STATUS::blank;
            
            if (!__get(__argv[1]))
                return RESPONSE_STATUS::key_notfound;
            break;
        case menuChoice::put:
            if (__argc != 3)
                return RESPONSE_STATUS::blank;
            if (!__put(__argv[1], __argv[2]))
                return RESPONSE_STATUS::key_existed;
            break;
        case menuChoice::del:
            if (__argc != 2)
                return RESPONSE_STATUS::blank;
            if (!__del(__argv[1]))
                return RESPONSE_STATUS::key_notfound;
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
    return __vector;
}

bool TCPListener::__get(std::string key)
{
    if (__database.empty()) return false;
    if (__database.count(key) == 0) return false;

    __send((__database[key] + "\r\n").data(), __database[key].size() + 2);
    return true;
}

bool TCPListener::__put(std::string  key, std::string val)
{
    if (__database.count(key) > 0) return false;

    __database[key] = val;
    return true;
}

bool TCPListener::__del(std::string key)
{
    if (__database.empty()) return false;
    return __database.erase(key);
}