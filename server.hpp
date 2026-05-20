#include <sys/socket.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

class server
{
    private:
        int _serverSocket;
        std::vector<pollfd> _fds;
    public:
        server() {};
        void init();
        void run();
        void acceptClient();
        void recieveData(int fd);
        void clientDesconected(int fd);
        void sendMessage(std::string message, int fd);
};