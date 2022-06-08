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

// #include <stdbool.h>
// for PATH_MAX
#include <limits.h>

#define SERVERPORT 8989
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

// prototypes
void * handle_connection(int);
int check(int, const char *);
int accept_new_connection(int);
int setup_server(short port, int backlog);


int main(int argc, char** argv)
{
    int server_socket = setup_server(SERVERPORT, SERVER_BACKLOG);

    int max_socket_so_far;
    fd_set current_sockets, ready_sockets;

    // initialize the current set
    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);
    max_socket_so_far = server_socket;

    std::cout << "FD_SETSIZE = " << FD_SETSIZE << std::endl;

    while (true)
    {
        // because select is destructive
        ready_sockets = current_sockets;

        if (select(server_socket, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < server_socket; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {
                if (i == server_socket)
                {
                    // this is a new connection
                    int client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets);
                    if (client_socket > max_socket_so_far)
                    {
                        max_socket_so_far = client_socket;
                    }
                }
                else
                {
                    handle_connection(i);
                    FD_CLR(i, &current_sockets);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

int setup_server(short port, int backlog)
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
            "Failed to create socket");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
            "Bin failed!");
    check(listen(server_socket, backlog),
            "Listen failed");
    
    return server_socket;
}

int accept_new_connection(int server_socket)
{
    int addr_size = sizeof(SA_IN);
    int client_socket;
    SA_IN client_addr;
    check(client_socket = 
            accept(server_socket,
                    (SA*)&client_addr,
                    (socklen_t*)&addr_size),
                    "accept failed");

    return client_socket;
}

int check(int exp, const char *msg)
{
    if (exp == SOCKETERROR)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}

void * handle_connection(int client_socket)
{
    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize = 0;

    char actualpath[PATH_MAX + 1];

    while ((bytes_read == read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1)))
    {
        msgsize += bytes_read;
        if (msgsize > BUFSIZE - 1 || buffer[msgsize - 1] == '\n') break;
    }

    check(bytes_read, "recv error");
    buffer[msgsize-1] = 0; // null terminate the message and remove the \n

    std::cout << "Request " <<  buffer << std::endl;
    fflush(stdout);

    if (realpath(buffer, actualpath) == NULL)
    {
        std::cout << "ERROR(bad path): " << buffer << std::endl;
        close(client_socket);
        return NULL;
    }

    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL)
    {
        std::cout << "ERROR(open): " << buffer << std::endl;
        close(client_socket);
        return NULL;
    }

    while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0)
    {
        write(client_socket, buffer, bytes_read);
    }
    close(client_socket);
    fclose(fp);
    std::cout << "Closing connection\n";
    return NULL;
}