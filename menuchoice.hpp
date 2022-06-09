#include <iostream>
#include <string>

enum class menuChoice
{
    quit,
    help,
    get,
    put,
    del,
    getfile,
    putfile,
    delfile,
    secretcommand,
    wrong
};

class ChoiceClass
{
public:
    static std::string menu;
    static void printMenu();
    static std::string getMenu();
    static menuChoice hashTheChoice(std::string choice);
};


std::string ChoiceClass::menu = R"(
**********                                              **********
syntax: [Function]

[Function]:
    [Q] or [QUIT]                   to quit the application
    [H] or [HELP]                   to print this menu
    [G] or [GET] _key_              to get the data by a key
    [P] or [PUT] _key_ _value_      to add new pair of <key, value> to the database
    [D] or [DEL] _key_              to del the pair from the database by a key
    [GF] or [GETFILE] _filename_                    to get the file by filename
    [PF] or [GETFILE] _filename_                    to put a new file to the database
    [DF] or [GETFILE] _filename_                    to del the file from the database by filename
    
)";

void ChoiceClass::printMenu() 
{
    std::cout << menu;
}

std::string ChoiceClass::getMenu() { return menu;    }

menuChoice ChoiceClass::hashTheChoice(std::string choice)
{
    if (choice == "Q" || choice == "QUIT")
        return menuChoice::quit;
    if (choice == "H" || choice == "HELP")
        return menuChoice::help;
    if (choice == "G" || choice == "GET")
        return menuChoice::get;
    if (choice == "P" || choice == "PUT")
        return menuChoice::put;
    if (choice == "D" || choice == "DEL")
        return menuChoice::del;
    if (choice == "GF" || choice == "GETFILE")
        return menuChoice::getfile;
    if (choice == "PF" || choice == "PUTFILE")
        return menuChoice::putfile;
    if (choice == "DF" || choice == "DELFILE")
        return menuChoice::delfile;
    if (choice == "SECRETCOMMAND")
        return menuChoice::secretcommand;

    return menuChoice::wrong;
}