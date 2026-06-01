#include <sys/socket.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include "Server.hpp"

int main()
{
    Server server;
    server.init();
    server.run();
    return (0);
}