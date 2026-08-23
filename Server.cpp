#include "Server.hpp"
#include "utils.hpp"

void Server::init()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(6667);
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
            exit(0); // hay que borrar todo

        if (_fds[0].revents & POLLIN)
            acceptClient();
        for (size_t i = 1; i < _fds.size(); i++)
        {
            if (_fds[i].revents & POLLIN)
                recieveData(_fds[i].fd);
        }
        if (_fds.size() == 1) // cuando no hay nadie conectado apaga lo puedes quitar
            break ;
    }
}
void Server::acceptClient()
{
    int clientSocket = accept(_fds[0].fd, NULL, NULL);
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
    _clients.push_back(nclient);
    std::cout << "new client has joined" << std::endl;
}
void Server::recieveData(int fd) //falta parseo bueno porque e pueden ppasar varios comandos a la vez aprovecha el buffer de la clase client
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
    /*if (nick == "")
    {
        message = "client ";
        std::stringstream ss;
        ss << fd;
        message += ss.str();
    }
    else
        message = nick;
    message.append(": ");
    message.append(buffer);
    if (message.size() > 10 && message.substr(10, 5) == "NICK ") // aqui pondras seguramente los comandos
    {
        addNick(message.substr(15, bytes - 6), fd);
        return ;
    }
    std::cout << "client" << fd << ": " << buffer;
    std::cout << "message " << message << std::endl;*/
    //sendMessage(message, fd);
    
}
void Server::runCommand(IRCmd command, Client *c){
    if (c->isUnReg()){
        if(command.cmd == "PASS"){
            if (c->getNickSet() ^ c->getUserSet()){
                std::string message = "462 ERR_ALREADYREGISTERED\r\n";
                send(c->getFd(), message.c_str(), message.size(), 0);
                //clientDesconected(c->getFd());
                return ;
            }
            else if (c->getNickSet() && c->getUserSet()){
                std::string message = "464 PASSWDMISMATCH\r\n";
                send(c->getFd(), message.c_str(), message.size(), 0);
                clientDesconected(c->getFd());
                return ;
            }
            c->setPass();
        }
        else if (command.cmd == "NICK"){
            if (c->getUserSet() && !c->getPassSet()){
                std::string message = "464 PASSWDMISMATCH\r\n";
                send(c->getFd(), message.c_str(), message.size(), 0);
                clientDesconected(c->getFd());
                return ;
            }
            else {
                c->setNick(command.params[0]);
                c->setNickSet();
            } 
        }
        else if (command.cmd == "USER"){
            if (c->getNickSet() && !c->getPassSet()){
                std::string message = "464 PASSWDMISMATCH\r\n";
                send(c->getFd(), message.c_str(), message.size(), 0);
                clientDesconected(c->getFd());
                return ;
            }else {
                c->setUser(command.params[0]);
                c->setUserSet();
            } 
        }
        else{
            std::string message = "Not registered\r\n";
            send(c->getFd(), message.c_str(), message.size(), 0);
            clientDesconected(c->getFd());
            return ;
        }
        if (c->getNickSet() && c->getUserSet() && c->getPassSet())
            {
                c->setReg();
                std::string message = "001 Registered correctly, welcome!\r\n";
                send(c->getFd(), message.c_str(), message.size(), 0);
            }
    } else {
        if (command.cmd == "PASS" || command.cmd == "USER"){}
        else if (command.cmd == "PRIVMSG") cmdMsg();
        else if (command.cmd == "JOIN") cmdJoin();
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
            std::cout << "client" << " desconected" << std::endl;
            return ;
        }
    }
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
        {
            close(fd);
            _clients.erase(_clients.begin() + i);
            std::cout << "client" << " desconected" << std::endl;
            return ;
        }
    }
}
void Server::sendMessage(std::string message, int fd)
{
    for (size_t i = 1; i < _fds.size(); i++)
    {
        if (_fds[i].fd != fd)
            send(_fds[i].fd, message.c_str(), message.size(), 0);
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