#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
//struct store address information
#include <netinet/in.h>
// closing the connected socket
#include <unistd.h>
// for inet_pton, convert the ip addr format
#include <arpa/inet.h>


int main(int argc, char const* argv[])
{
    // creating a socket
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    // address to connect to
    std::string ip_address = argv[1]; 
    int port = atoi(argv[2]);
    std::cout << "Connecting to " << ip_address << " : " << port << std::endl; 

    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(port);
    // remote_address.sin_addr.s_addr = INADDR_ANY;
    // inet_pton(AF_INET, "127.0.0.1", &remote_address.sin_addr.s_addr);
    inet_pton(AF_INET, ip_address.c_str(), &remote_address.sin_addr.s_addr);

    // Connecting
    bool socket_status = connect(client_socket, reinterpret_cast<struct sockaddr *>(&remote_address), sizeof(remote_address));
    // bool socket_status = connect(client_socket, (struct sockaddr *)&remote_address, sizeof(remote_address));
    std::cout << "socket binded successfully" << std::endl;

    enum NetworkConnectStatus {
        Succeeded = 0,
        Failed = -1
    };
    // check for the error
    if (socket_status == NetworkConnectStatus::Failed)
    {
        std::cout << "There was an error making a connection to the remote socket\n";
        exit(0);
    }
    // receive data from the server
    char server_response[1024]; // the buffer to receive
    std::cout << "Server response length: " << sizeof(server_response) << std::endl;
    recv(client_socket, &server_response, sizeof(server_response), 0);

    // print out the server's response
    std::cout << "Server response length: " << sizeof(server_response) << std::endl;
    printf("Server response: %s\n", server_response);
    std::cout << "Server response length: " << sizeof(server_response) << std::endl;

    // std::cout << "The server sent the data: " << server_response << std::endl;

    // and then close the socket
    close(client_socket);


    return 0;
}