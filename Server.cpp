#include "Server.hpp"
#include "utils.hpp"

void Server::init()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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
        updatePollEvents();
        if (poll(_fds.data(), _fds.size(), -1) < 0)
            exit(0);

        if (_fds[0].revents & POLLIN)
            acceptClient();
        for (size_t i = 1; i < _fds.size(); )
        {
            int fd = _fds[i].fd;
            if (_fds[i].revents & (POLLERR | POLLNVAL))
            {
                clientDesconected(fd);
            }
            else if (_fds[i].revents & POLLIN)
            {
                recieveData(fd);
                if (i < _fds.size() && _fds[i].fd == fd)
                    ++i;
            }
            else if (_fds[i].revents & POLLOUT)
            {
                sendPendingData(fd);
                ++i;
            }
            else if (_fds[i].revents & POLLHUP)
            {
                clientDesconected(fd);
            }
            else
                ++i;
        }
        /*if (_fds.size() == 1)
            break ;*/
    }
}
void Server::acceptClient()
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientSocket = accept4(_fds[0].fd, (struct sockaddr*)&clientAddr,
        &addrLen, SOCK_NONBLOCK);
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
    if (cli == NULL)
        return ;
    cli->addBuffer(buffer);
    while ((pos = cli->getBuff().find("\r\n")) != std::string::npos){
        std::string commandStr = cli->getBuff().substr(0,pos);
        cli->setBuffer(cli->getBuff().erase(0, pos+2));
        IRCmd cmd = getCommand(commandStr);
        runCommand(cmd, cli);
        if (searchClient(fd) == NULL)
            return ;
    }    
}
void Server::runCommand(IRCmd command, Client *c){
    std::cout << "cmd.cmd: " << command.cmd << std::endl;
    for (size_t i = 0; i < command.params.size(); i++){
        std::cout << "cmd.params: " << command.params[i] << std::endl;
    }
    if (c->isUnReg()){
        if(command.cmd == "PASS"){
            if (command.params.size() < 1){
                c->sendMessage(461, "PASS :Not enough parameters");
                return ;
            }
            if (c->getNickSet() && c->getUserSet()){
                c->sendMessage(462, ":Unauthorized command (already registered)");
                clientDesconected(c->getFd());
                return ;
            }
            if (command.params[0] != _password){
                c->sendMessage(464, ":Password incorrect");
                clientDesconected(c->getFd());
                return ;
            }
            c->setPass();
        }
        else if (command.cmd == "NICK"){
            if (command.params.size() < 1){
                c->sendMessage(431, ":No nickname given");
                return ;
            }
            if (c->getUserSet() && !c->getPassSet()){
                c->sendMessage(464, ":Password incorrect");
                clientDesconected(c->getFd());
                return ;
            }
            else {
                for(size_t i = 0; i < _clients.size(); i++){
                    if (command.params[0] == _clients[i].getNick()){
                        c->sendMessage(433, ":Nickname is already in use");
                        return ;
                    }
                }
                c->setNick(command.params[0]);
                c->setNickSet();
            } 
        }
        else if (command.cmd == "USER"){
            if (command.params.size() < 1){
                c->sendMessage(461, "USER :Not enough parameters");
                return ;
            }
            if (c->getNickSet() && !c->getPassSet()){
                c->sendMessage(464, ":Password incorrect");
                clientDesconected(c->getFd());
                return ;
            }else {
                c->setUser(command.params[0]);
                c->setUserSet();
            } 
        } else if (command.cmd == "CAP"){
            if (command.params.size() > 0 && command.params[0] == "LS"){
                sendMessage(":server CAP * LS :\r\n", c->getFd());
            }
        }
        else{
            c->sendMessage(451, ":You have not registered");
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
        }
        else if (command.cmd == "PRIVMSG") {
            cmdMsg(command, c, this);
        }
        else if (command.cmd == "JOIN") {
            cmdJoin(command, c, this);
        }
        else if (command.cmd == "MODE"){
            cmdMode(command, c, this);
        } else if (command.cmd == "INVITE"){
            cmdInvite(command, c, this);
        } else if (command.cmd == "KICK"){
            cmdKick(command, c, this);
        }
        else if (command.cmd == "TOPIC"){
            cmdTopic(command, c, this);
        } else if (command.cmd == "NICK"){
            if (command.params.size() < 1){
                c->sendMessage(431, ":No nickname given");
                return ;
            }
            for(size_t i = 0; i < _clients.size(); i++){
                if (command.params[0] == _clients[i].getNick()){
                    c->sendMessage(433, ":Nickname is already in use");
                    return ;
                }
            }
            c->setNick(command.params[0]);
        }
        else if (command.cmd != "CAP" && command.cmd != "WHO"){
            c->sendMessage(421, c->getNick() + " " + command.cmd + " :Unknown command");
            return;
        }
    }
}

void Server::clientDesconected(int fd)
{
    std::string nickname;
    Client *client = searchClient(fd);
    if (client != NULL)
        nickname = client->getNick();

    for (size_t i = 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == fd)
        {
            _fds.erase(_fds.begin() + i);
            break;
        }
    }
    if (!nickname.empty())
    {
        for (size_t i = 0; i < _channels.size(); i++)
            _channels[i].removeUser(nickname);
    }
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
        {
            _clients.erase(_clients.begin() + i);
            break ;
        }
    }
    close(fd);
    std::cout << "client" << " disconected" << std::endl;
}
void Server::sendMessage(std::string message, int fd)
{
    Client *client = searchClient(fd);
    if (client != NULL)
        client->addOutput(message);
}
void Server::sendPendingData(int fd)
{
    Client *client = searchClient(fd);
    if (client == NULL || client->getOutput().empty())
        return ;
    std::string &output = client->getOutput();
    int bytes = send(fd, output.c_str(), output.size(), 0);
    if (bytes > 0)
        output.erase(0, bytes);
}
void Server::updatePollEvents()
{
    for (size_t i = 1; i < _fds.size(); i++)
    {
        Client *client = searchClient(_fds[i].fd);
        _fds[i].events = POLLIN;
        if (client != NULL && !client->getOutput().empty())
            _fds[i].events |= POLLOUT;
    }
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