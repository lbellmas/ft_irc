#include "Channel.hpp"

Channel::Channel(std::string name){
    _name = name;
    _topic = "";
    _key = "";
    _user_limit = -1;
    _invite_only = false;
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