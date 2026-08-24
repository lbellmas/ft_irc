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
            //message channel
        } else {
            //error message to client
        }
    } else {
        Client *reciever;
        std::cout << "Nick to send: " << command.params[0] << std::endl;
        if ((reciever = s->searchClient(command.params[0])) != NULL){
            std::cout << "Reciever name :" << reciever->getNick() << std::endl;
            std::string prefix = ":" + c->getNick() + "!" + c->getUser() + "@" + c->getHostname() + " ";
            s->sendMessage(prefix + command.params[1] + "\n\r", c->getFd());
        } else {
            s->sendMessage("Error sending message! Nick not in use", c->getFd());
        }
    }
}
/*void cmdJoin(IRCmd command, Client *c, Server *){

}*/

/*
std::cout << "cmd.cmd: " << cmd.cmd << std::endl;
for (size_t i = 0; i < cmd.params.size(); i++){
std::cout << "cmd.params: " << cmd.params[i] << std::endl;
}
*/