#include <sys/socket.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include "Server.hpp"

int main(int ac, char *argv[])
{
    if (ac != 3) {
        std::cout << "Wrong number of parameters. Run as follows: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    int port = std::atoi(argv[1]);
    if (port <= 1024 || port > 65535){
        std::cout << "Incorrect port!" << std::endl;
        return 2;
    }
    std::string password = argv[2];
    if (password.empty()){
        std::cout << "Password cannot be empty!" << std::endl;
        return 3;
    }
    Server server(port, password);
    server.init();
    server.run();
    return (0);
}