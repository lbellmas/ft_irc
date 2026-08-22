#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include "Client.hpp"
#include <vector>

class Channel{
    private:
        std::string _name;
        std::vector<int> _clientFds;
    public:
        Channel(std::string name);
        std::string getName() const;
        void addClient(int fd);
        void removeClient(int fd);
        bool hasClient(int fd) const;
        std::vector<int > getClients() const;
};

#endif