#include "tcpclient.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include "menuchoice.hpp"

TCPClient::TCPClient()
    : raw_ip_addr("127.0.0.1"), raw_port("8080")
{
}

TCPClient::TCPClient(const char* ip_addr, const char* port)
    : raw_ip_addr(ip_addr), raw_port(port)
{
}

bool TCPClient::__init()
{
    // create a socket
    if (!__create_socket()) return false;

    // connect to a server
    if (!__connect()) return false;

    // try to communicate to test the network
    if(!__testing()) return false;


    return true;
}

bool TCPClient::__testing()
{
    // send request message
    __send(__header.data(), __header.size());

    int bytesRecv = __recv();
    while (bytesRecv <= 0)
    {
        __send(__header.data(), __header.size());
        bytesRecv = __recv();
    }
    std::cout << std::string(__recv_buf, 0, bytesRecv);
    return true;
}

bool TCPClient::__create_socket()
{
    if ((this->__socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) 
            == SOCKETERROR)
    {
        std::cerr << "Can't create a socket" << std::endl;
		return false;
    }
    return true;
}

void TCPClient::__define_address()
{
    std::string IP_ADDR = raw_ip_addr;
    int PORT = atoi(raw_port);
    // int PORT = 8080;
    __remote_address.sin_family = AF_INET;
    __remote_address.sin_port = htons(PORT);
    // server_address.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, IP_ADDR.c_str(), &__remote_address.sin_addr.s_addr);
}

bool TCPClient::__connect()
{
    __define_address();

    if (connect(__socket, reinterpret_cast<struct sockaddr *>(&__remote_address), sizeof(__remote_address))
        == SOCKETERROR)
    {
        std::cerr << "Can't connect to IP/PORT" << std::endl;
		return false;
    }
    return true;
}

int TCPClient::__recv()
{
    __recv_buf.clear();
    __recv_buf.resize(BUFFSIZE);

    // int bytesRecv = recv(__socket, &__recv_buf, sizeof(__recv_buf), 0);
    int bytesRecv = recv(__socket, __recv_buf.data(), __recv_buf.size(), 0);
    if (bytesRecv == SOCKETERROR)
    {
        std::cerr << "There was a connection issue!" << std::endl;
        close(EXIT_FAILURE);
    }
    if (bytesRecv == 0)
    {
        std::cout << "The server disconnected" << std::endl;
        close(EXIT_FAILURE);
    }
    return bytesRecv;
}

void TCPClient::__send(char *msg, int len)
{
    send(__socket, msg, len, 0);
}

void TCPClient::__send_int(uint32_t &num, int len)
{
    send(__socket, &num, len, 0);
}

std::vector<std::string> get_argv(std::string big_string, char split)
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

void TCPClient::__run()
{
    while (true)
    {
        // std::cout << "Waiting for new command" << std::endl;
        if (__ioHandler->READINGFILE_BIT)
        {
            // std::cout << "Reading file chunks" << std::endl;
            __read_file_chunk();
        }
        else if (__ioHandler->WRITINGFILE_BIT)
        {
            do
            {
                // std::cout << "Writing file chunks" << std::endl;
            } while (__write_file_chunk());
        }
        else
        {
            // clear the buffer
            __send_buf.clear();

            // wait for input
            std::getline(std::cin, __send_buf);
            // __send_buf = *it;

            std::vector<std::string> __argv = get_argv(__send_buf, ' ');
            int __argc = __argv.size();
            if (__argc == 2)
            {
                std::string command = __argv[0];
                std::string filename = __argv[1];

                switch (ChoiceClass::hashTheChoice(command))
                {
                    case menuChoice::putfile:
                    {
                        if (!__uploadfile(filename))
                            continue;
                        // __send_buf += "\r\n";
                        // __send(__send_buf.data(), __send_buf.size());
                        break;
                    }
                    case menuChoice::getfile:
                    {
                        if (!__downloadfile(filename))
                            continue;
                        break;
                    }
                    default:
                        // std::cout << "Unknown choice" << std::endl;
                        break;
                }
            }

            // not send other command if file is downloading
            if (__ioHandler->WRITINGFILE_BIT)
                continue;
            else
            {
                __send_buf += "\r\n";
                __send(__send_buf.data(), __send_buf.size());
            }
        }
        // std::cout << "Sending " << send_buf;
        std::cout << "Sent, wait for receiving" << std::endl;

        // wait for the message
        int bytesRecv = __recv();

        // std::string response = std::string(__recv_buf, 0, bytesRecv);
        std::string response(__recv_buf.begin(), __recv_buf.begin() + bytesRecv);

        std::cout << "[Received:] " << response;
        if (strcmp("Closing connection, bye\r\n", response.c_str()) == 0)
        {
            std::cout << "Closing connection" << std::endl;
            __send(std::string("").data(), 0);
            break;
        }
    }
    // close the socket
    close(__socket);
}



bool TCPClient::__uploadfile(std::string filename)
{
    if (!__ioHandler->open_to_read(filename))
        return false;

    // get length
    long length = __ioHandler->get_file_length();

    std::cout << "sending length: " << length << std::endl;

    __send_buf += " " + std::to_string(length);

    return true;
}

bool TCPClient::__downloadfile(std::string filename)
{
    // send get command
    __send_buf += "\r\n";
    __send(__send_buf.data(), __send_buf.size());
    // recv length

    int bytesRecv = __recv();
    std::string response = std::string(__recv_buf, 0, bytesRecv);
    std::cout << "Received (before download): " << response;

    if (strcmp(response.c_str(), "File not found\r\n") == 0)
    {
        return false;
    }

    std::vector<std::string> __argv = get_argv(response, ' ');

    std::cout << "Recv file length: " << __ioHandler->file_size << std::endl;
    std::string msg = "Recv file length, ready to read\r\n";
    __send(msg.data(), msg.size());

    if (!__ioHandler->open_to_write("client_" + filename))
        return false;
    __ioHandler->set_file_size(__argv[1]);

    return true;
}

void TCPClient::__read_file_chunk()
{
    if (__ioHandler->read_file())
    {
        // std::cout << ifs.gcount() << std::endl;
        __ioHandler->buffer.resize( BUFFSIZE );
        std::cout << "Sending size: " << BUFFSIZE << std::endl;
        __send(reinterpret_cast<char*>(__ioHandler->buffer.data()), __ioHandler->buffer.size());
        __ioHandler->buffer.resize( BUFFSIZE );
    }
    else
    {
        // std::cout << leftbytes << std::endl;
        if (__ioHandler->leftbytes > 0)
        {
            __ioHandler->read_file(__ioHandler->leftbytes);
            std::cout << "Sending size: " << __ioHandler->leftbytes << std::endl;
            __send(reinterpret_cast<char*>(__ioHandler->buffer.data()), __ioHandler->leftbytes);
        }

        std::cout << "Completed sending files" << std::endl;
        
        __ioHandler->close_file();
    }
/*
            std::vector<BYTE> buffer(std::istreambuf_iterator<char>(ifs), {});
            std::cout << "sending " << buffer.size() << std::endl;
            __send(reinterpret_cast<char*>(buffer.data()), buffer.size());

            std::cout << "Completed sending files" << std::endl;
            ifs.close();
            READINGFILE_BIT = false;
*/
}

bool TCPClient::__write_file_chunk()
{
    __ioHandler->buffer.clear();
    __ioHandler->buffer.resize(BUFFSIZE);

    int bytesRecv = recv(__socket, __ioHandler->buffer.data(), __ioHandler->buffer.size(), 0);

    __ioHandler->write_file(bytesRecv);

    if (__ioHandler->is_finish_writing())
    {
        std::string msg = {"Completely written the file"};
        std::cout << msg << std::endl;

        __send_buf = msg;
        __send(msg.data(), msg.size());

        __ioHandler->close_file();
        return false;
    }
    else
    {
        std::string msg = "Received " + std::to_string(bytesRecv) + "\r\n";
        std::cout << msg;
        __send(msg.data(), msg.size());
        return true;
    }
}