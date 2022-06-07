#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

std::vector<std::string> get_argv(std::string big_string, char split)
{
    std::string temp_string;
    std::vector<std::string> __vector;
    std::stringstream ss(big_string);
    while (std::getline(ss, temp_string, split))
        __vector.push_back(temp_string);
    return __vector;
}

int main()
{
    std::unordered_map<std::string, std::string> __map;
    __map["test"] = "val1";
    __map["test"] = "val2";
    std::unordered_map<std::string, std::string>::iterator it;

    for (it = __map.begin(); it != __map.end(); it++)
        std::cout << it->first << "  " << it->second << std::endl;

    return EXIT_SUCCESS;
}