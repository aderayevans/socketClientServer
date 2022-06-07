#include <iostream>
#include <string>

enum class menuChoice
{
    quit,
    help,
    get,
    put,
    del,

    wrong
};

void printMenu();
std::string getMenu();
menuChoice hashTheChoice(std::string choice);

std::string menu = R"(
**********                                              **********
syntax: [Function]

[Function]:
    [Q] or [QUIT]                   to quit the application
    [H] or [HELP]                   to print this menu
    [G] or [GET] _key_              to get the data by a key
    [P] or [PUT] _key_ _value_      to add new pair of <key, value> to the database
    [D] or [DEL] _key_              to del the pair from the database by a key
    
)";

void printMenu() 
{
    std::cout << menu;
}

std::string getMenu() { return menu;    }

menuChoice hashTheChoice(std::string choice)
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

    return menuChoice::wrong;
}