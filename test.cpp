#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>
// for inet_pton, convert the ip addr format
#include <arpa/inet.h>

typedef unsigned char BYTE;


int main()
{
    // std::string filename("image2201x1467.jpg");
    std::string filename("image3840x2160.jpeg");
    std::cout << filename << std::endl;
    int chunk = 128;
    // int chunk = 50;
    std::ifstream ifs;
    ifs.open(filename, std::ios::binary | std::ios::in);
    if (!ifs.is_open())
        std::cout << "failed to open" << std::endl;

    // get length
    ifs.seekg (0, std::ios::end);
    int length = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<std::vector<BYTE>> vectorOfVector;
    std::vector<BYTE> __vector(chunk);
    std::cout << length << std::endl;

    int leftbytes = length % chunk;
    while(ifs.read(reinterpret_cast<char*>(__vector.data()), __vector.size())) 
    {
        vectorOfVector.push_back(__vector);

        std::cout << ifs.gcount() << std::endl;
        // __vector.resize( ifs.gcount() );

        // proc( v );

        __vector.resize( chunk );
    }
    std::cout << leftbytes << std::endl;
    if (leftbytes > 0)
    {
        ifs.read(reinterpret_cast<char*>(__vector.data()), leftbytes);
        vectorOfVector.push_back(__vector);
    }

    // for (auto &val:vectorOfVector)
        //sending each packet / when done send message putfileDone

    ifs.close();
    /*
    Writing
    */
    std::ofstream ofs;
    ofs.open("new_" + filename, std::ios::binary | std::ios::out | std::ios::ate);
    if (!ofs.is_open())
        std::cout << "failed to open to write" << std::endl;

    for (auto &val:vectorOfVector)
        ofs.write(reinterpret_cast<char*>(val.data()), val.size());

    ofs.close();

    return EXIT_SUCCESS;
}