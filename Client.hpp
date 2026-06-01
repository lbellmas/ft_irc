#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

class Client
{
    private:
        int fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string buffer;
        enum ClientStatus
        {
            CONNECTED,
            AUTHENTICATED,
            REGISTERED
        };
        std::vector<std::string> channels;
        bool isOperator;
    public:
        Client();
        Client(int nfd);
        ~Client();
        void setFd(int nfd);
        void setNick(std::string nick);
        void setUser(std::string user);
        void setReal(std::string real);
        void setBuffer(std::string buff);
        void setStatus(int status);
        void setOperator(bool status);

        void addChannels(std::string chann);

        int getFd();
        std::string getNick();
        std::string getUser();
        std::string getReal();
        std::string getBuff();
        int getStatus();
        std::vector<std::string> getChannels();
        bool getOperator();
};

#endif