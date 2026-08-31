#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include "Client.hpp"
#include <vector>
#include "utils.hpp"

struct IRCmd;
class Server;

class Channel{
    private:
        std::string _name;
        std::string _key;
        std::string _topic;
        std::vector<std::string> _clients;
        std::vector<std::string> _invited;
        std::vector<std::string> _operators;
        int _user_limit;
        bool _invite_only;
        bool _topic_restricted;
        
        
    public:
        Channel(std::string name);
        std::string getName() const;
        void addClient(std::string client);
        void addOperator(std::string client);
        void removeClient(std::string client);
        bool hasClient(std::string client) const;
        bool isInviteOnly() const;
        std::string getTopic() const;
        void changeTopic(std::string newTopic);
        std::vector<std::string> getClients() const;
        bool isInvited(std::string name);
        bool isOperator(std::string client);
        void changeModes(IRCmd command, Client *c, Server *s);
        int getNumUsers() const;
        int getUserLimit() const;
        
        
};

#endif