#ifndef IOHANDLER_HPP
#define IOHANDLER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <unistd.h>

typedef unsigned char BYTE;
const int BUFFSIZE = 1024;
const int PROGRESSBAR_LENGTH = 30;

struct IOHandler
{
    bool WRITINGFILE_BIT = false, READINGFILE_BIT = false;
    int __client_socket;
    std::ifstream ifs;
    std::ofstream ofs;
    std::string filename;
    long file_size = 0;
    long file_left_size = 0;
    int leftbytes;
    std::vector<BYTE> buffer;
    // bool lock_output;

    // IOHandler(bool lock_output=false)
        // : lock_output(lock_output)
    // {
    // }
    bool open_to_write(std::string);
    bool open_to_read(std::string);
    long get_file_length();

    void write_file(int);
    bool read_file(int size=BUFFSIZE);

    void set_file_size(std::string);

    void close_file();

    void recal_file_left_size(int);
    void update_bar(int, long);

    bool is_finish_writing();
};

#endif