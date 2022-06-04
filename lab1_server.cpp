#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
//struct store address information
#include <netinet/in.h>
// closing the connected socket
#include <unistd.h>
#include <string.h>
#include <string>

int main(int argc, char const* argv[])
{
    char server_message[1024] = "You have reached the server!";
    std::cout << "Server message length: " << sizeof(server_message) << std::endl;

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
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    std::cout << "Server successfully created\n";

    // define the server address
    int port = 8080;
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    // INADDR_ANY = 0.0.0.0
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    bind(server_socket, reinterpret_cast<struct sockaddr *>(&server_address), sizeof(server_address));
    // bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    std::cout << "socket binded to " << port << std::endl;

    listen(server_socket, 5);
    std::cout << "socket is listening\n";

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    // send data to the client
    send(client_socket, server_message, sizeof(server_message), 0);

    // and then close the socket
    close(server_socket);
    // closing the listening socket
	shutdown(server_socket, SHUT_RDWR);

    return 0;
}
