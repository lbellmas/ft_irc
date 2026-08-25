#include "utils.hpp"
#include <cstdlib>


IRCmd getCommand(std::string buffer){
    IRCmd cmd;
    std::string params;
    std::string prefix;
    std::istringstream ss(buffer);
    if (buffer[0] == ':'){
        ss >> prefix;
        prefix.erase(0,1);
    }
    ss >> cmd.cmd;
    for (size_t i = 0; i < cmd.cmd.size(); i++){
        cmd.cmd[i] = toupper(cmd.cmd[i]);
    }
    while (ss >> params){
        if (params[0] == ':'){
            std::string left;
            std::getline(ss, left);
            cmd.params.push_back(params.erase(0,1) + left);
            break;
        }
        cmd.params.push_back(params);
    }
    return cmd;
}

void cmdMsg(IRCmd command, Client *c, Server *s){
    if (command.params[0][0] == '#'){
        Channel *chan;
        if((chan = s->searchChannel(command.params[0])) != NULL){
            if (chan->hasClient(c->getFd())){
                std::vector<int> clients = chan->getClients();
                for(size_t i = 0; i < clients.size(); i++){
                    if (clients[i] != c->getFd())
                        s->sendMessage(c->getPrefix() + command.params[1] + "\n\r", clients[i]);
                }
            }
        } else {
            //error message to client
        }
    } else {
        Client *reciever;
        std::cout << "Nick to send: " << command.params[0] << std::endl;
        if ((reciever = s->searchClient(command.params[0])) != NULL){
            std::cout << "Reciever name :" << reciever->getNick() << std::endl;
            s->sendMessage(c->getPrefix() + command.params[1] + "\n\r", reciever->getFd());
        } else {
            s->sendMessage("Error sending message! Nick not in use", reciever->getFd());
        }
    }
}
void cmdJoin(IRCmd command, Client *c, Server *s){ //STILL NEED TO CHEKC USER LIMIT
    Channel *chan;
    if((chan = s->searchChannel(command.params[0])) != NULL){
        if (chan->isInviteOnly()){ //NEED TO CHECK USER LIMIT
            if (chan->isInvited(c->getNick())){
                chan->addClient(c->getFd());
            } else {
                //SEND ERROR MESSAGE
            }
        } else {
            if (!chan->hasClient(c->getFd()))
                chan->addClient(c->getFd());
            else {
                //ERROR MESASGE
            }
        }
    } else {
        std::cout << "Created Channel!" << std::endl;
        Channel Ch(command.params[0]);
        Ch.addClient(c->getFd());
        s->addChannel(Ch);
    }
}

/*
std::cout << "cmd.cmd: " << cmd.cmd << std::endl;
for (size_t i = 0; i < cmd.params.size(); i++){
std::cout << "cmd.params: " << cmd.params[i] << std::endl;
}
*/