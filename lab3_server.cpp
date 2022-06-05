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

int main(int argc, char const* argv[])
{
    // create a socket
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) 
            == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't create a socket" << std::endl;
		exit(EXIT_FAILURE);
    }
    // bind the socket to a ip / port
    std::string IP_ADDR = argv[1];
    int PORT = atoi(argv[2]);
    // int PORT = 8080;
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    // server_address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, IP_ADDR.c_str(), &server_address.sin_addr.s_addr);

    if (bind(server_socket, reinterpret_cast<sockaddr *>(&server_address), sizeof(server_address))
        == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't bind to IP/PORT" << std::endl;
		exit(EXIT_FAILURE);
    }
    // mark the socket for listening
    if (listen(server_socket, SOMAXCONN) == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't listen!" << std::endl;
		exit(EXIT_FAILURE);
    }
    // accept the call
    sockaddr_in client_address;
    socklen_t clientSize = sizeof(client_address);
    int client_socket;
    
    client_socket = accept(server_socket, 
                            reinterpret_cast<sockaddr *>(&client_address), 
                            &clientSize);

    if (client_socket == NetworkConnectStatus::Failed)
    {
        std::cerr << "Problem with client connecting!" << std::endl;
        exit(EXIT_FAILURE);
    }
    // close the listening socket
    close(server_socket);

    char host[NI_MAXHOST] = {'\0'};
    char service[NI_MAXSERV] = {'\0'};

    // memset(host, 0, NI_MAXHOST);
    // memset(service, 0, NI_MAXSERV);

    int result = getnameinfo(reinterpret_cast<sockaddr *>(&client_address),
                                sizeof(client_address),
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
        inet_ntop(AF_INET, &client_address.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client_address.sin_port) << std::endl;
    }

    // send and receive
    char buf[4096] = {'\0'};

    while (true)
    {
        // clear the buffer
        memset(buf, 0, 4096);
        // wait for the message
        int bytesRecv = recv(client_socket, buf, sizeof(buf), 0);
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

        // display message
        std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;

        // resend message
        send(client_socket, buf, bytesRecv + 1, 0);
    }
    // close the socket
    close(client_socket);

    return EXIT_SUCCESS;
}