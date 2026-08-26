#include "Channel.hpp"

Channel::Channel(std::string name){
    _name = name;
    _topic = "";
    _key = "";
    _user_limit = -1;
    _invite_only = false;
    _topic_restricted = false;
}

std::string Channel::getName() const {
    return _name;
}

bool Channel::hasClient(int fd) const{
    for (std::size_t i = 0; i < _clientFds.size(); i++){
        if (fd == _clientFds[i]) return true;
    }
    return false;
}

std::vector<int> Channel::getClients() const{
    return _clientFds;
}

void Channel::addClient(int fd){
    _clientFds.push_back(fd);
}

void Channel::removeClient(int fd){
    for (std::vector<int>::iterator it = _clientFds.begin(); it != _clientFds.end(); it++){
        if (*it == fd){
            _clientFds.erase(it);
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

bool Channel::isOperator(int fd){
    for(size_t i = 0; i < _operators.size(); i++){
        if (_operators[i] == fd) return true;
    }
    return false;
}

void Channel::addOperator(int fd){
    _operators.push_back(fd);
}

void Channel::changeModes(IRCmd command, Client *c, Server *s){
    bool type;
    if (command.params[0][0] != '+' && command.params[0][0] != '-'){
        // ERROR MESSAGE
        return ;
    } 
    int paramsIndex = 2;
    for (size_t i = 0; i < command.params[0].size(); i++){
        if (command.params[0][i] == '+') type = true;
        else if (command.params[0][i] == '-') type = false;
        else if (command.params[0][i] == 'i') _invite_only = type;
        else if (command.params[0][i] == 't') _topic_restricted = type;
        else if (command.params[0][i] == 'k'){
            if (type) _key = command.params[paramsIndex++];
            else _key = "";
        }
        else if (command.params[0][i] == 'o') {
            /*if (type) {
                _operators.push_back()
            }
            for(size_t i = 0; i < _operators.size(); i++){
                Client *c = s->searchClient(_operators[i]);
                if ()
            }*/
        }
        else if (command.params[0][i] == 'l') type = false;


        
    }
}