#include "Server.hpp"
#include "utils.hpp"

void Server::init()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(_port);
    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(_serverSocket, (struct sockaddr*)&direccion, sizeof(direccion)) < 0)
        exit(-1);
    pollfd serverPoll;
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    _fds.push_back(serverPoll);
}
void Server::run()
{
    listen(_serverSocket, 3);

    while (true)
    {
        if (poll(_fds.data(), _fds.size(), -1) < 0)
            exit(0);

        if (_fds[0].revents & POLLIN)
            acceptClient();
        for (size_t i = 1; i < _fds.size(); i++)
        {
            if (_fds[i].revents & POLLIN)
                recieveData(_fds[i].fd);
        }
        if (_fds.size() == 1)
            break ;
    }
}
void Server::acceptClient()
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientSocket = accept(_fds[0].fd, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket < 0)
    {
        std::cout << "client has filed joining" << std::endl;
        return ;
    }
    pollfd clientPoll;
    clientPoll.fd = clientSocket;
    clientPoll.events = POLLIN;
    _fds.push_back(clientPoll);
    Client nclient(clientSocket);
    nclient.setHostname(inet_ntoa(clientAddr.sin_addr));
    _clients.push_back(nclient);
    std::cout << "new client has joined" << std::endl;
}
void Server::recieveData(int fd)
{
    char buffer[1024];
    std::size_t pos;
    int bytes = recv(fd, buffer, 1023, 0);
    if (bytes <= 0)
        return (clientDesconected(fd));
    buffer[bytes] = '\0';
    Client *cli = searchClient(fd);
    cli->addBuffer(buffer);
    while ((pos = cli->getBuff().find("\r\n")) != std::string::npos){
        std::string commandStr = cli->getBuff().substr(0,pos);
        cli->setBuffer(cli->getBuff().erase(0, pos+2));
        IRCmd cmd = getCommand(commandStr);
        runCommand(cmd, cli);
    }    
}
void Server::runCommand(IRCmd command, Client *c){
    std::cout << "cmd.cmd: " << command.cmd << std::endl;
    for (size_t i = 0; i < command.params.size(); i++){
        std::cout << "cmd.params: " << command.params[i] << std::endl;
    }
    if (c->isUnReg()){
        if(command.cmd == "PASS"){
            if (c->getNickSet() && c->getUserSet()){
                c->sendMessage(462, ":Unauthorized command (already registered)");
                clientDesconected(c->getFd());
                return ;
            }
            if (command.params[0] != _password){
                c->sendMessage(464, ":Password incorrect");
                clientDesconected(c->getFd());
                //return ;
            }
            c->setPass();
        }
        else if (command.cmd == "NICK"){
            if (c->getUserSet() && !c->getPassSet()){
                c->sendMessage(464, ":Password incorrect");
                //send(c->getFd(), message.c_str(), message.size(), 0);
                clientDesconected(c->getFd());
                return ;
            }
            else {
                for(size_t i = 0; i < _clients.size(); i++){
                    if (command.params[0] == _clients[i].getNick()){
                        c->sendMessage(433, ":Nickname is already in use");
                        //send(c->getFd(), message.c_str(), message.size(), 0);
                        return ;
                    }
                }
                c->setNick(command.params[0]);
                c->setNickSet();
            } 
        }
        else if (command.cmd == "USER"){
            if (c->getNickSet() && !c->getPassSet()){
                c->sendMessage(464, ":Password incorrect");
                //std::string message = "464 PASSWDMISMATCH\r\n";
                //send(c->getFd(), message.c_str(), message.size(), 0);
                clientDesconected(c->getFd());
                return ;
            }else {
                c->setUser(command.params[0]);
                c->setUserSet();
            } 
        } else if (command.cmd == "CAP"){
            if (command.params[0] == "LS"){
                sendMessage(":server CAP * LS :\r\n", c->getFd());
            }
        }
        else{
            std::cout << "cmd.cmd: " << command.cmd << std::endl;
            for (size_t i = 0; i < command.params.size(); i++){
                std::cout << "cmd.params: " << command.params[i] << std::endl;
            }
            c->sendMessage(451, ":You have not registered");
            //std::string message = "Not registered\r\n";
            //send(c->getFd(), message.c_str(), message.size(), 0);
            clientDesconected(c->getFd());
            return ;
        }
        if (c->getNickSet() && c->getUserSet() && c->getPassSet())
            {
                c->setReg();
                c->sendMessage(1, ":Welcome to the Internet Relay Network " + c->getPrefix().erase(0,1));
            }
    } else {
        if (command.cmd == "PASS" || command.cmd == "USER"){
            c->sendMessage(462, ":Unauthorized command (already registered)");
            //std::string message = "462 ERR_ALREADYREGISTERED\r\n";
            //send(c->getFd(), message.c_str(), message.size(), 0);
            //return;
        }
        else if (command.cmd == "PRIVMSG") {
            cmdMsg(command, c, this);
        }
        else if (command.cmd == "JOIN") {
            cmdJoin(command, c, this);
        }
        else if (command.cmd == "MODE"){
            cmdMode(command, c, this);
        } 
        else if (command.cmd == "TOPIC"){
            //cmdTopic(command, c, this);
        }
        else if (command.cmd != "CAP"){
            std::string message = "UNKNOWN COMMAND\r\n";
            send(c->getFd(), message.c_str(), message.size(), 0);
            return;
        }
    }
}

void Server::clientDesconected(int fd)
{
    for (size_t i = 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == fd)
        {
            close(fd);
            _fds.erase(_fds.begin() + i);
            std::cout << "client" << " disconected" << std::endl;
            break;
        }
    }
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
        {
            close(fd);
            _clients.erase(_clients.begin() + i);
            std::cout << "client" << " disconected" << std::endl;
            break ;
        }
    }
}
void Server::sendMessage(std::string message, int fd)
{
    send(fd, message.c_str(), message.size(), 0);
}
void Server::addNick(std::string nick, int fd)
{
    std::cout << "nick added: " << nick << std::endl;
    Client *client = searchClient(fd);
    if (client == NULL)
        return ;
    (*client).setNick(nick);
    std::cout << "Nick" << client->getNick() << std::endl;
}
Client *Server::searchClient(int fd)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
            return (&_clients[i]);
    }
    std::cout << "no clients found" << std::endl;
    return (NULL);
}
Client *Server::searchClient(std::string nick)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getNick() == nick)
            return (&_clients[i]);
    }
    std::cout << "no clients found" << std::endl;
    return (NULL);
}

Channel *Server::searchChannel(std::string cn){
    for (size_t i = 0; i < _channels.size(); i++){
        if (cn == _channels[i].getName()){
            return (&_channels[i]);
        }
    }
    return NULL;
}

void Server::addChannel(Channel c){
    _channels.push_back(c);
}