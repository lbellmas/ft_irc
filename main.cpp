#include <sys/socket.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include "server.hpp"

int main()
{
    server Server;
    Server.init();
    Server.run();
    return (0);
}