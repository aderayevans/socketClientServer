#include "iohandler.hpp"



bool IOHandler::open_to_write(std::string filename)
{
    buffer.clear();
    buffer.resize(BUFFSIZE);

    ofs.open(filename, std::ios::binary | std::ios::out | std::ios::ate);
    if (!ofs.is_open())
    {
        std::cout << "failed to open to write" << std::endl;
        ofs.close();
        return false;
    }
    WRITINGFILE_BIT = true;
    return true;
}

bool IOHandler::open_to_read(std::string filename)
{
    buffer.clear();
    buffer.resize(BUFFSIZE);

    ifs.open(filename, std::ios::binary | std::ios::in);
    if (!ifs.is_open())
    {
        std::cerr << "failed to open" << std::endl;
        ifs.close();
        return false;
    }
    READINGFILE_BIT = true;

    ifs.seekg (0, std::ios::end);
    file_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    leftbytes = file_size % BUFFSIZE;


    return true;
}

long IOHandler::get_file_length()
{
    return file_size;
}

void IOHandler::set_file_size(std::string len_str)
{
    long file_size = std::stol( len_str );
    // std::cout << "Size: " << file_size << std::endl;
    this->file_size = file_size;
    this->file_left_size = file_size;
    this->WRITINGFILE_BIT = true;
}

void IOHandler::write_file(int bytesRecv)
{
    recal_file_left_size(bytesRecv);
    std::vector<BYTE> __vector(buffer.begin(), buffer.begin() + bytesRecv);
    ofs.write(reinterpret_cast<char*>(__vector.data()), __vector.size());
}

bool IOHandler::read_file(int size)
{
    ifs.read(reinterpret_cast<char*>(buffer.data()), size);
    return !ifs.fail();
}

void IOHandler::close_file()
{
    if (ofs.is_open())
        ofs.close();
    if (ifs.is_open())    
        ifs.close();

    WRITINGFILE_BIT = false;
    READINGFILE_BIT = false;
}

void IOHandler::update_bar(int percent)
{
    int num = percent * PROGRESSBAR_LENGTH  / 100;
    std::cout << "\r[";
    for (int i = 0; i < num; ++i)
    {
        std::cout << "#";
    }for (int i = 0; i < PROGRESSBAR_LENGTH - num; ++i)
    {
        std::cout << " ";
    }
    std::cout << "] " << percent << "%";
    if (percent == 100)
        std::cout << " Done\r\n";
}

void IOHandler::recal_file_left_size(int bytes)
{
    // std::cout << "[Bytes recv]: " << bytes << std::endl;
    // std::cout << "[?...?] " << file_left_size << " - " << bytes << std::endl;
    file_left_size -= bytes;
    if (file_left_size < 0) file_left_size = 0;
    double percent = ((double)(file_size - file_left_size)/file_size);
    update_bar(percent*100);
}

bool IOHandler::is_finish_writing()
{
    return (file_left_size <= 0);
}