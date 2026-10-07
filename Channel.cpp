#include "Channel.hpp"

Channel::Channel(std::string name){
    _name = name;
    _topic = "";
    _key = "";
    _user_limit = 0;
    _invite_only = false;
    _topic_restricted = false;
}

std::string Channel::getName() const {
    return _name;
}

bool Channel::hasClient(std::string client) const{
    for (std::size_t i = 0; i < _clients.size(); i++){
        if (client == _clients[i]) return true;
    }
    return false;
}

std::vector<std::string> Channel::getClients() const{
    return _clients;
}

void Channel::addClient(std::string client){
    _clients.push_back(client);
}

void Channel::removeClient(std::string client){
    for (std::vector<std::string>::iterator it = _clients.begin(); it != _clients.end(); it++){
        if (*it == client){
            _clients.erase(it);
            return;
        }
    }
}

void Channel::removeUser(std::string client){
    std::vector<std::string>::iterator it;

    for (it = _clients.begin(); it != _clients.end(); ){
        if (*it == client)
            it = _clients.erase(it);
        else
            ++it;
    }
    for (it = _operators.begin(); it != _operators.end(); ){
        if (*it == client)
            it = _operators.erase(it);
        else
            ++it;
    }
    for (it = _invited.begin(); it != _invited.end(); ){
        if (*it == client)
            it = _invited.erase(it);
        else
            ++it;
    }
}

bool Channel::isInviteOnly() const {
    return _invite_only;
}

bool Channel::isKeyNeeded() const {
    return _key != "";
}

bool Channel::isKeyCorrect(std::string k) const{
    std::cout << k <<  "  "   << _key << std::endl;
    return k == _key;
}

std::string Channel::getTopic() const {
    return _topic;
}

void Channel::changeTopic(std::string newTopic) {
    _topic = newTopic;
}

bool Channel::isInvited(std::string name){
    for(size_t i = 0; i < _invited.size(); i++){
        if (_invited[i] == name) return true;
    }
    return false;
}

bool Channel::isOperator(std::string client){
    for(size_t i = 0; i < _operators.size(); i++){
        if (_operators[i] == client) return true;
    }
    return false;
}

void Channel::addOperator(std::string client){
    _operators.push_back(client);
}

void Channel::changeModes(IRCmd command, Client *c){
    bool type = false;
    if (command.params.size() > 1){
        if (command.params[1].empty()){
            c->sendMessage(472, " :is unknown mode char to me");
            return ;
        }
        if (command.params[1][0] != '+' && command.params[1][0] != '-'){
            c->sendMessage(472, std::string(1, command.params[1][0]) + " :is unknown mode char to me");
            return ;
        } 
        int paramsIndex = 2;
        for (size_t i = 0; i < command.params[1].size(); i++){
            if (command.params[1][i] == '+') type = true;
            else if (command.params[1][i] == '-') type = false;
            else if (command.params[1][i] == 'i') _invite_only = type;
            else if (command.params[1][i] == 't') _topic_restricted = type;
            else if (command.params[1][i] == 'k'){
                if (size_t(paramsIndex) < command.params.size()){
                    std::cout << "Type " << type << std::endl;
                    if (type) {
                        if (_key != ""){
                            c->sendMessage(476, getName() + " :Channel key already set  ");
                            return ;
                        }
                        _key = command.params[paramsIndex++];
                    }
                    else _key = "";
                } else {
                    c->sendMessage(461, command.cmd + " :Not enough parameters");
                    return ;
                }
                std::cout << "key " << _key << std::endl;
            }
            else if (command.params[1][i] == 'o') {
                if (size_t(paramsIndex) < command.params.size()){
                    if (hasClient(command.params[paramsIndex])){
                        if (type) {
                            _operators.push_back(command.params[paramsIndex++]);
                        } else {
                            std::vector<std::string>::iterator it;
                            for(it = _operators.begin(); it != _operators.end(); ++it){
                                if (*it == command.params[paramsIndex]) {
                                    _operators.erase(it);
                                    break;
                                }
                            }
                            if (it == _operators.end()){
                                c->sendMessage(482, getName() + " :You're not channel operator");
                            }
                            paramsIndex++;
                        }
                    } else {
                        c->sendMessage(442, getName() + " :You're not on that channel");
                    }
                } else {
                    c->sendMessage(461, command.cmd + " :Not enough parameters");
                    return ;
                }
            }
            else if (command.params[1][i] == 'l'){
                if (size_t(paramsIndex) < command.params.size()){
                    if (type) {
                        int limit = std::atoi(command.params[paramsIndex++].c_str());
                        if (limit > 0) _user_limit = limit;
                        else _user_limit = 0;
                    }
                    else _user_limit = 0;
                } else {
                    c->sendMessage(461, command.cmd + " :Not enough parameters");
                    return ;
                }
            } 
        }
    }
    std::cout << "TYPE: " << type << std::endl;
    std::cout << "IsInvite only? " << _invite_only << std::endl; 
}

int Channel::getNumUsers() const{
    return _clients.size();
}

int Channel::getUserLimit() const {
    return _user_limit;
}

void Channel::invite(std::string nick) {
    _invited.push_back(nick);
}

bool Channel::wasInvited(std::string nick) const{
    for(size_t i = 0; i < _invited.size(); i++){
        if (_invited[i] == nick) return true;
    }
    return false;
}

void Channel::broadcast(std::string message, Server *s){
    for(size_t i = 0; i < _clients.size(); i++){
        
        Client *client = s->searchClient(_clients[i]);
        if (client != NULL)
            s->sendMessage(message, client->getFd());
    }
}

bool Channel::isTopicRestricted() const {
    return _topic_restricted;
}