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
            if (chan->hasClient(c->getNick())){
                std::vector<std::string> clients = chan->getClients();
                for(size_t i = 0; i < clients.size(); i++){
                    if (clients[i] != c->getNick())
                        s->sendMessage(c->getPrefix() + command.params[1] + "\n\r", s->searchClient(clients[i])->getFd());
                }
            } else {
                s->sendMessage("ERROR, USER NOT PART OF CHANNEL", c->getFd());
            }
        } else {
            s->sendMessage("ERROR, CHANNEL DOES NOT EXIST", c->getFd());
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
                chan->addClient(c->getNick());
            } else {
                s->sendMessage("ERROR, TRYING TO JOIN INVITE ONLY CHANNEL", c->getFd());
            }
        } else {
            if (!chan->hasClient(c->getNick()))
                chan->addClient(c->getNick());
            else {
                s->sendMessage("ERROR, USER ALREADY IN CHANNEL", c->getFd());
            }
        }
    } else {
        std::cout << "Created Channel!" << std::endl;
        Channel Ch(command.params[0]);
        std::cout << Ch.getName() << std::endl;
        Ch.addClient(c->getNick());
        Ch.addOperator(c->getNick());
        s->addChannel(Ch);
    }
}

void cmdMode(IRCmd command, Client *c, Server *s){
    if (command.params[0][0] == '#'){
        Channel *ch;
        if ((ch = s->searchChannel(command.params[0])) != NULL){
            if (ch->isOperator(c->getNick())){
                
            } else if (ch->hasClient(c->getNick())){
                //NOT AUTHORIZED (NOT OPERATOR OF THE CHANNEL)
            } else {
                //ISNT IN CHANNEL
            }
        } else {
            //CHANNEL DOES NOT EXIST
        }
    }
}

/*void cmdTopic(IRCmd command, Client *c, Server *s){

}*/

/*void cmdKick(IRCmd command, Client *c, Server *s){

}

void cmdInvite(IRCmd command, Client *c, Server *s){

}*/ 
/*
std::cout << "cmd.cmd: " << cmd.cmd << std::endl;
for (size_t i = 0; i < cmd.params.size(); i++){
std::cout << "cmd.params: " << cmd.params[i] << std::endl;
}
*/