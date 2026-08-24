#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include "Channel.hpp"

struct IRCmd;

class Server
{
    private:
        int _serverSocket;
        std::vector<pollfd> _fds;
        std::vector<Client> _clients;
        std::vector<Channel> _channels;
    public:
        Server() {};
        void init();
        void run();
        void acceptClient();
        void recieveData(int fd);
        void clientDesconected(int fd);
        void sendMessage(std::string message, int fd);
        void addNick(std::string, int fd);
        void runCommand(IRCmd command, Client * c);
        Client *searchClient(int fd);
        Client *searchClient(std::string nick);
        Channel *searchChannel(std::string cn);
};
#endif