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
    for (std::size_t i = 0; i < _clientFds.size(); i++){
        if (client == _clientFds[i]) return true;
    }
    return false;
}

std::vector<std::string> Channel::getClients() const{
    return _clientFds;
}

void Channel::addClient(std::string client){
    _clientFds.push_back(client);
}

void Channel::removeClient(std::string client){
    for (std::vector<std::string>::iterator it = _clientFds.begin(); it != _clientFds.end(); it++){
        if (*it == client){
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
    if (command.params[0][0] != '+' && command.params[0][0] != '-'){
        std::string message = ": is unkown mode char to me";
        s->sendMessage(message.insert(0, 1, command.params[0][0]), c->getFd());
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
}