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
        std::string name = command.params[0];
        if((chan = s->searchChannel(command.params[0])) != NULL){
            if (chan->hasClient(c->getNick())){ 
                std::string message = c->getPrefix().append(" " + command.cmd + " " + command.params[0] + " :" + command.params[1]).append("\r\n");
                std::vector<std::string> clients = chan->getClients();
                for(size_t i = 0; i < clients.size(); i++){
                    if (clients[i] != c->getNick())
                        s->sendMessage(message, s->searchClient(clients[i])->getFd());
                }
            } else {
                c->sendMessage(404, ":Cannot send to channel");
            }
        } else {
            c->sendMessage(403, command.params[0] + " :No such channel");
        }
    } else {
        Client *reciever;
        if ((reciever = s->searchClient(command.params[0])) != NULL){
            std::string message = c->getPrefix().append("PRIVMSG " + reciever->getNick() + " :").append(command.params[1]).append("\r\n");
            s->sendMessage(message, reciever->getFd());
        } else {
            c->sendMessage(401, command.params[0] + " :No such nick/channel");
        }
    }
}
void cmdJoin(IRCmd command, Client *c, Server *s){ 
    Channel *chan;
    if (command.params[0][0] == '#'){
        if((chan = s->searchChannel(command.params[0])) != NULL){
            if (chan->getNumUsers() + 1 <= chan->getUserLimit() || chan->getUserLimit() == 0){
                std::cout << "chanInvONlyy: " << chan->isInviteOnly() << std::endl;
                if (chan->isInviteOnly()){
                    if (chan->isInvited(c->getNick())){
                        chan->addClient(c->getNick());
                        c->addChannels(chan->getName());
                        std::string message = c->getPrefix() + command.cmd + " " + command.params[0].append("\r\n");
                        s->sendMessage(message, c->getFd());
                    } else {
                        c->sendMessage(473, ":Cannot join channel (+i)");
                    }
                } else {
                    if (!chan->hasClient(c->getNick())){
                        chan->addClient(c->getNick());
                        c->addChannels(chan->getName());
                        std::string message = c->getPrefix() + command.cmd + " " + command.params[0].append("\r\n");
                        s->sendMessage(message, c->getFd());
                    }
                    else {
                        c->sendMessage(443, c->getNick() + " " + chan->getName() + " :is already on channel");
                    }
                }
            } else {
                c->sendMessage(471, ":Cannot join channel (+l)");
            }
        } else {
            std::cout << "Created Channel!" << std::endl;
            Channel Ch(command.params[0]);
            Ch.addClient(c->getNick());
            Ch.addOperator(c->getNick());
            c->addChannels(command.params[0]);
            s->addChannel(Ch);
            std::string message = c->getPrefix() + command.cmd + " " + command.params[0].append("\r\n");
            s->sendMessage(message, c->getFd());
        }
    } else if (command.params[0] == "0"){
        //LEAVE ALL CHANNELS
    } else {
        c->sendMessage(482, c->getNick() + " " + command.params[0] + " :Bad channel mask");
    }
}

void cmdMode(IRCmd command, Client *c, Server *s){
    if (command.params[0][0] == '#'){
        Channel *ch;
        if ((ch = s->searchChannel(command.params[0])) != NULL){
            if (ch->isOperator(c->getNick())){
                ch->changeModes(command, c, s);
            } else if (ch->hasClient(c->getNick())){
                c->sendMessage(482, ch->getName() + ":You're not channel operator");
            } else {
                c->sendMessage(442, ch->getName() + ":You're not on that channel");
            }
        } else {
            c->sendMessage(403, ":No such channel");
        }
    }
}

/*void cmdTopic(IRCmd command, Client *c, Server *s){

}*/

void cmdKick(IRCmd command, Client *c, Server *s){
    Channel *ch = s->searchChannel(command.params[0]);
    if (ch != NULL){
            Client *cli = s->searchClient(command.params[1]);
            if (cli != NULL){
                if (!ch->hasClient(c->getNick())){
                    c->sendMessage(442, ch->getName() + ":You're not on that channel");
                }
                if (!ch->isOperator(c->getNick())){
                    c->sendMessage(482, ch->getName() + ":You're not channel operator");
                }
                if (!ch->hasClient(cli->getNick())){
                    c->sendMessage(441, cli->getNick() + " " + ch->getName() + " :They aren't on that channel");
                }
                ch->removeClient(cli->getNick());
            } else {
                c->sendMessage(401, ":No such nick/channel");
            }
    } else {
        c->sendMessage(403, ":No such channel");
    }
}

void cmdInvite(IRCmd command, Client *c, Server *s){
    Channel *ch = s->searchChannel(command.params[1]);
    if (ch != NULL){
        Client *cli = s->searchClient(command.params[0]);
        if (cli != NULL){
            if (!ch->hasClient(c->getNick())){
                c->sendMessage(442, ch->getName() + ":You're not on that channel");
            }
            else if (!ch->isOperator(c->getNick()) && ch->isInviteOnly()){
                c->sendMessage(482, ch->getName() + ":You're not channel operator");
            }
            else if (ch->hasClient(cli->getNick())){
                c->sendMessage(443, cli->getNick() + " " + ch->getName() + " :is already on channel");
            }
            else if (!ch->wasInvited(cli->getNick())){
                ch->invite(cli->getNick());
            }
            
        } else {
            c->sendMessage(401, ":No such nick/channel");
        }
    } else {
        c->sendMessage(403, ":No such channel");
    }
} 
/*
std::cout << "cmd.cmd: " << cmd.cmd << std::endl;
for (size_t i = 0; i < cmd.params.size(); i++){
std::cout << "cmd.params: " << cmd.params[i] << std::endl;
}
*/