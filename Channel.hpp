#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include "Client.hpp"
#include <vector>
#include "utils.hpp"

class Channel{
    private:
        std::string _name;
        std::string _key;
        std::string _topic;
        std::vector<int> _clientFds;
        std::vector<std::string> _invited;
        std::vector<int> _operators;
        int _user_limit;
        bool _invite_only;
        bool _topic_restricted;
        
        
    public:
        Channel(std::string name);
        std::string getName() const;
        void addClient(int fd);
        void addOperator(int fd);
        void removeClient(int fd);
        bool hasClient(int fd) const;
        bool isInviteOnly() const;
        std::string getTopic() const;
        void changeTopic(std::string newTopic);
        std::vector<int> getClients() const;
        bool isInvited(std::string name);
        bool isOperator(int fd);
        void changeModes(IRCmd command, Client *c, Server *s);
        
};

#endif