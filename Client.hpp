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
#include <sstream>
#include <iomanip>

class Client
{
    private:
        int fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string buffer;
        bool isPassSet;
        bool isUserSet;
        bool isNickSet;
        std::string hostName;
        enum ClientStatus
        {
            UNREGISTERED,
            CONNECTED,
            AUTHENTICATED,
            REGISTERED
        };
        ClientStatus status;
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
        void addBuffer(std::string buff);

        void setStatus(int status);
        void setOperator(bool status);

        void addChannels(std::string chann);

        int getFd();
        std::string getNick();
        std::string getUser();
        std::string getReal();
        std::string getBuff();
        int getStatus() const;
        std::vector<std::string> getChannels();
        bool getOperator();
        bool getPassSet();
        bool getNickSet();
        bool getUserSet();
        void setNickSet();
        void setUserSet();
        bool isAuth();
        bool isUnReg();
        void setPass();
        void setReg();
        void setHostname(std::string hn);
        std::string getHostname() const;
        std::string getPrefix();
        void sendMessage(int code, std::string message);
        void addChannel(std::string ch);

};

#endif