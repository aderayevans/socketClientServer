#include <iostream>
#include <string.h>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
//struct store address information
#include <netinet/in.h>
// closing the connected socket
#include <unistd.h>

int main(int argc, char const* argv[])
{
    // open a file to serve
    FILE *html_data = fopen("index.html", "r");

    // char response_data[1024];
    char buffer[128];
    char response_data[2048] = {'\0'}; //Initialize to '\0' so there's no garbage values since we will append to the end of this string
    while(fgets(buffer, sizeof(buffer), html_data)){
        strcat(response_data, buffer);
    }
    // fgets(response_data, sizeof(response_data), html_data);

    char http_header[2048] = "HTTP/1.0 200 OK\r\n\n";
    strcat(http_header, response_data);
    std::cout << http_header << std::endl;

    // create a socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    std::cout << "Server successfully created\n";

    // define the server address
    int port = 8001;
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
    while (true) 
    {
        client_socket = accept(server_socket, NULL, NULL);

        // send data to the client
        send(client_socket, http_header, sizeof(http_header), 0);

        // and then close the socket
        close(server_socket);
    }


    return EXIT_SUCCESS;
}