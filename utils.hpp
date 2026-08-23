#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>

struct IRCmd{
    std::string cmd;
    std::vector<std::string> params;
};

IRCmd getCommand(std::string buffer);

#endif