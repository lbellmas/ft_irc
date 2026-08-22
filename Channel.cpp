#include "Channel.hpp"

Channel::Channel(std::string name){
    _name = name;
}

std::string Channel::getName() const {
    return _name;
}

bool Channel::hasClient(int fd) const{
    for (int i = 0; i < _clientFds.size(); i++){
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