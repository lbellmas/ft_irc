#include "utils.hpp"
#include <cstdlib>
#include <algorithm>


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
    if (command.params.size() < 2){
        c->sendMessage(461, "PRIVMSG :Not enough parameters");
        return ;
    }
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
                c->sendMessage(404, command.params[0] + " :Cannot send to channel");
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

static std::vector<std::string> splitComma(const std::string& s) {
    std::vector<std::string> out;
    size_t start = 0, end;
    while ((end = s.find(',', start)) != std::string::npos) {
        out.push_back(s.substr(start, end - start));
        start = end + 1;
    }
    out.push_back(s.substr(start));
    return out;
}


static void joinSingleChannel(Client *c, Server *s, const std::string& chanName, const std::string& key) {
    if (chanName.empty() || chanName[0] != '#') {
        c->sendMessage(476, chanName + " :Bad Channel Mask");
        return;
    }

    Channel *chan = s->searchChannel(chanName);

    if (chan == NULL) {
        std::cout << "Created Channel!" << std::endl;
        Channel newChan(chanName);
        newChan.addClient(c->getNick());
        newChan.addOperator(c->getNick());
        c->addChannels(chanName);
        s->addChannel(newChan);

        std::string msg = c->getPrefix() + "JOIN " + chanName + "\r\n";
        s->sendMessage(msg, c->getFd());
        return;
    }

    if (chan->hasClient(c->getNick())) {
        c->sendMessage(443, c->getNick() + " " + chanName + " :is already on channel");
        return;
    }

    if (chan->getUserLimit() != 0 && chan->getNumUsers() + 1 > chan->getUserLimit()) {
        c->sendMessage(471, chanName + " :Cannot join channel (+l)");
        return;
    }

    if (chan->isInviteOnly() && !chan->isInvited(c->getNick())) {
        c->sendMessage(473, chanName + " :Cannot join channel (+i)");
        return;
    }

    if (chan->isKeyNeeded() && !chan->isKeyCorrect(key)) {
        c->sendMessage(475, chanName + " :Cannot join channel (+k)");
        return;
    }

    chan->addClient(c->getNick());
    c->addChannels(chanName);

    std::string msg = c->getPrefix() + "JOIN " + chanName + "\r\n";
    s->sendMessage(msg, c->getFd());
}

void cmdJoin(IRCmd command, Client *c, Server *s) {
    if (command.params.empty()) {
        c->sendMessage(461, "JOIN :Not enough parameters");
        return;
    }

    std::vector<std::string> channels = splitComma(command.params[0]);
    std::vector<std::string> keys;
    if (command.params.size() > 1)
        keys = splitComma(command.params[1]);

    for (size_t i = 0; i < channels.size(); ++i) {
        std::string key = (i < keys.size()) ? keys[i] : "";
        joinSingleChannel(c, s, channels[i], key);
    }
}

void cmdMode(IRCmd command, Client *c, Server *s){
    Channel *ch;
    if (command.params.size() < 2){
        c->sendMessage(461, "MODE :Not enough parameters");
        return ;
    }
    std::cout << "cmd.params.size(): " << command.params.size() << std::endl;
    //if (command.params.size() > 2){
        if ((ch = s->searchChannel(command.params[0])) != NULL){
            std::cout << "Channel found" << std::endl;
            if (ch->isOperator(c->getNick())){
                ch->changeModes(command, c);
            } else if (ch->hasClient(c->getNick())){
                c->sendMessage(482, ch->getName() + " :You're not channel operator");
            } else {
                c->sendMessage(442, ch->getName() + " :You're not on that channel");
            }
        } else {
            std::cout << "Channel not found" << std::endl;
            c->sendMessage(403, ch->getName() + " :No such channel");
        }
    //}
}

void cmdTopic(IRCmd command, Client *c, Server *s){
    Channel *ch;
    if (command.params.size() < 1){
        c->sendMessage(461, "TOPIC :Not enough parameters");
        return ;
    }
    if ((ch = s->searchChannel(command.params[0])) != NULL){
        if (ch->hasClient(c->getNick())){
            std::cout << "CLient in channel!\n"; 
            if (command.params.size() < 2){
                std::string topic = ch->getTopic();
                if (topic == "")
                    c->sendMessage(331, ch->getName() + " :No topic is set");
                else
                    c->sendMessage(332, ch->getName() + " :" + topic);
            } else {
                if (ch->isOperator(c->getNick()) && !ch->isTopicRestricted()){
                    ch->changeTopic(command.params[1]);
                } else {
                    c->sendMessage(482, ch->getName() + " :You're not channel operator");
                    return ;
                }
                std::string mess = c->getPrefix() + "TOPIC " + ch->getName() + " :" + command.params[1] + "\r\n";
                std::vector<std::string> clients = ch->getClients();
                for (size_t i = 0; i < clients.size(); i++){
                    Client *cl = s->searchClient(clients[i]);
                    s->sendMessage(mess, cl->getFd());
                }
            }
        } else {
            c->sendMessage(442, ch->getName() + " :You're not on that channel");
        }
    } else {
        c->sendMessage(403, ch->getName() + " :No such channel");   
    }
}

void cmdKick(IRCmd command, Client *c, Server *s){
    Channel *ch = s->searchChannel(command.params[0]);
    if (ch != NULL){
            Client *cli = s->searchClient(command.params[1]);
            if (cli != NULL){
                if (!ch->hasClient(c->getNick())){
                    c->sendMessage(442, ch->getName() + " :You're not on that channel");
                }
                if (!ch->isOperator(c->getNick())){
                    c->sendMessage(482, ch->getName() + " :You're not channel operator");
                }
                if (!ch->hasClient(cli->getNick())){
                    c->sendMessage(441, cli->getNick() + " " + ch->getName() + " :They aren't on that channel");
                }
                
                std::string reason = "";
                if (command.params.size() > 2) reason = command.params[2];
                std::vector<std::string> clients = ch->getClients();
                for (size_t i = 0; i < clients.size(); i++){
                    Client *cl = s->searchClient(clients[i]);
                    s->sendMessage(cl->getPrefix() + " " + command.cmd + " " + command.params[0] + " " + command.params[1] + " :" + reason +"\r\n", cl->getFd());
                }
                ch->removeClient(cli->getNick());
            } else {
                c->sendMessage(401, command.params[0] + " :No such nick/channel");
            }
    } else {
        c->sendMessage(403, command.params[0] + " :No such channel");
    }
}

void cmdInvite(IRCmd command, Client *c, Server *s){
    Channel *ch = s->searchChannel(command.params[1]);
    if (ch != NULL){
        Client *cli = s->searchClient(command.params[0]);
        if (cli != NULL){
            if (!ch->hasClient(c->getNick())){
                c->sendMessage(442, ch->getName() + " :You're not on that channel");
            }
            else if (!ch->isOperator(c->getNick()) && ch->isInviteOnly()){
                c->sendMessage(482, ch->getName() + " :You're not channel operator");
            }
            else if (ch->hasClient(cli->getNick())){
                c->sendMessage(443, cli->getNick() + " " + ch->getName() + " :is already on channel");
            }
            else if (!ch->wasInvited(cli->getNick())){
                ch->invite(cli->getNick());
            }
            
        } else {
            c->sendMessage(401, command.params[0] + " :No such nick/channel");
        }
    } else {
        c->sendMessage(403, command.params[0] + " :No such channel");
    }
} 
/*
std::cout << "cmd.cmd: " << cmd.cmd << std::endl;
for (size_t i = 0; i < cmd.params.size(); i++){
std::cout << "cmd.params: " << cmd.params[i] << std::endl;
}
*/