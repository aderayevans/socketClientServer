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
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) 
            == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't create a socket" << std::endl;
		exit(EXIT_FAILURE);
    }
    // connect to a server
    std::string IP_ADDR = argv[1];
    int PORT = atoi(argv[2]);
    // int PORT = 8080;
    sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(PORT);
    inet_pton(AF_INET, IP_ADDR.c_str(), &remote_address.sin_addr.s_addr);

    if (connect(client_socket, reinterpret_cast<struct sockaddr *>(&remote_address), sizeof(remote_address))
        == NetworkConnectStatus::Failed)
    {
        std::cerr << "Can't connect to IP/PORT" << std::endl;
		exit(EXIT_FAILURE);
    }

    // send and receive
    // char send_buf[4096] = {'\0'};
    char recv_buf[4096] = {'\0'};
    std::string send_buf;
    char request[] = "GET / HTTP/1.1\r\n\r\n";
    send(client_socket, request, sizeof(request), 0);

    while (true)
    {
        // clear the buffer
        send_buf.clear();
        memset(recv_buf, 0, 4096);
        // memset(send_buf, 0, 4096);


        // wait for the message
        int bytesRecv = recv(client_socket, &recv_buf, sizeof(recv_buf), 0);

        if (bytesRecv == NetworkConnectStatus::Failed)
        {
            std::cerr << "There was a connection issue!" << std::endl;
            break;
        }
        if (bytesRecv == 0)
        {
            std::cout << "The server disconnected" << std::endl;
            break;
        }
        std::cout << "Received: " << std::string(recv_buf, 0, bytesRecv);



        // wait for input
        // std::cin >> send_buf;
		std::getline(std::cin, send_buf);
        send_buf += "\r\n";
        if (send_buf.compare("QUIT\r\n") == 0) 
        {
            send(client_socket, "Close connection\r\n", 19, 0);
            break;
        }
        send(client_socket, send_buf.c_str(), send_buf.size(), 0);
        // std::cout << "Sending " << send_buf;
    }
    // close the socket
    close(client_socket);

    return EXIT_SUCCESS;
}