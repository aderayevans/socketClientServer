#include <iostream>
#include <fstream>

#include "handlefile.hpp"

template <class OBJECTFILE>
OBJECTFILE HandleFile::read_file(std::string filename)
{
    std::ifstream ifs;
    ifs.open(filename);

    OBJECTFILE obj;

    ifs >> obj;

    // ifs.read((char*)&obj, sizeof(obj));
    
    // while (!ifs.eof()) {
    //     ifs.read((char*)&obj, sizeof(obj));
    // }
    ifs.close();
    std::cout << "Read file succeeded!!!" << std::endl;
    return obj;
}

template <class OBJECTFILE>
std::vector<OBJECTFILE> HandleFile::read_file_multiple_obj(std::string filename)
{
    std::ifstream ifs;
    ifs.open(filename);

    std::vector<OBJECTFILE> objs;
    OBJECTFILE obj;
    while (ifs >> obj)
    {
        objs.push_back(obj);
    }
    ifs.close();
    std::cout << "Read file succeeded!!!" << std::endl;
    return objs;
}
    
template <class OBJECTFILE>
void HandleFile::write_file(std::string filename, OBJECTFILE obj, WRITER_MODE mode)
{
    std::ofstream ofs;
    if (mode == WRITER_MODE::APPEND)
    {
        ofs.open(filename, std::ios_base::app);
        ofs << obj;
        ofs.close();
        std::cout << "Append file succeeded!!!" << std::endl;
    }
    else if (mode == WRITER_MODE::CLEAR)
    {
        ofs.open(filename, std::ios::out | std::ios::trunc);
        ofs.close();
        std::cout << "Clear file content succeeded!!!" << std::endl;
    }
    else    
    {
        ofs.open(filename);
        ofs << obj;
        ofs.close();
        std::cout << "Write file succeeded!!!" << std::endl;
    }

}

