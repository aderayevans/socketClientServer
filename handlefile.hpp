#ifndef HANDLEFILE_HPP
#define HANDLEFILE_HPP

#include "string"
#include <vector>

enum class WRITER_MODE { APPEND, TRUNCATE, CLEAR };

class HandleFile
{
public:
    template <class OBJECTFILE>
    static OBJECTFILE read_file(std::string);
    
    template <class OBJECTFILE>
    static void write_file(std::string, OBJECTFILE, WRITER_MODE mode=WRITER_MODE::TRUNCATE);
    
    template <class OBJECTFILE>
    static std::vector<OBJECTFILE> read_file_multiple_obj(std::string);

private:

};

#endif