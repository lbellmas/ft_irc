#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include "Client.hpp"
#include <vector>

class Channel{
    private:
        std::string _name;
        std::vector<int> _clientFds;
        std::string _key;
        std::string _topic;
        int _user_limit;
        bool _invite_only;
        std::vector<std::string> _invited;
    public:
        Channel(std::string name);
        std::string getName() const;
        void addClient(int fd);
        void removeClient(int fd);
        bool hasClient(int fd) const;
        bool isInviteOnly() const;
        std::string getTopic() const;
        void changeTopic(std::string newTopic);
        std::vector<int> getClients() const;
        bool isInvited(std::string name);
        
};

#endif