#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include "Client.hpp"
#include "Server.hpp"



struct IRCmd{
    std::string cmd;
    std::vector<std::string> params;
};

class Server;

IRCmd getCommand(std::string buffer);
void cmdMsg(IRCmd command, Client *c, Server *s);
void cmdJoin(IRCmd command, Client *c, Server *s);

#endif