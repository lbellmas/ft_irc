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

bool Channel::isInviteOnly() const {
    return _invite_only;
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

void Channel::changeModes(IRCmd command, Client *c, Server *s){
    bool type;
    std::cout << "command.cmd: " << command.cmd << std::endl;
    for (size_t i = 0; i < command.params.size(); i++){
    std::cout << "command.params: " << command.params[i] << std::endl;
    }
    if (command.params[1][0] != '+' && command.params[1][0] != '-'){
        std::string message = ": is unkown mode char to me";
        s->sendMessage(message.insert(0, 1, command.params[1][0]), c->getFd());
        return ;
    } 
    int paramsIndex = 2;
    for (size_t i = 0; i < command.params[0].size(); i++){
        std::cout << "params[i] = " << command.params[0][i] << std::endl; 
        if (command.params[1][i] == '+') type = true;
        else if (command.params[1][i] == '-') type = false;
        else if (command.params[1][i] == 'i') _invite_only = type;
        else if (command.params[1][i] == 't') _topic_restricted = type;
        else if (command.params[1][i] == 'k'){
            if (type) _key = command.params[paramsIndex++];
            else _key = "";
        }
        else if (command.params[1][i] == 'o') {
            if (hasClient(command.params[paramsIndex])){
                if (type) {
                    _operators.push_back(command.params[paramsIndex++]);
                } else {
                    std::vector<std::string>::iterator it;
                    for(it = _operators.begin(); it != _operators.end(); it++){
                        if (*it == command.params[paramsIndex]) _operators.erase(it);
                    }
                    if (it == _operators.end()){
                        //ERROR MESSAGE, USER WAS NOT AN OPERATOR
                    }
                    paramsIndex++;
                }
            } else {
                //ERROR USER NOT IN CHANNEL
            }
        }
        else if (command.params[0][i] == 'l'){
            if (type) {
                int limit = std::atoi(command.params[paramsIndex++].c_str());
                if (limit > 0) _user_limit = limit;
                else _user_limit = 0;
            }
            else _user_limit = 0;
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
        
        send(s->searchClient(_clients[i])->getFd(), message.c_str(), message.size(), 0);
    }
}