#include "Server.hpp"

void Server::init()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in direccion;
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(6667);
    bind(_serverSocket, (struct sockaddr*)&direccion, sizeof(direccion));
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
        poll(_fds.data(), _fds.size(), -1);

        if (_fds[0].revents & POLLIN)
            acceptClient();
        for (int i = 1; i < _fds.size(); i++)
        {
            if (_fds[i].revents & POLLIN)
                recieveData(_fds[i].fd);
        }
        if (_fds.size() == 1) // cuando no hay nadie conectado apaga
            break ;
    }
}
void Server::acceptClient()
{
    int clientSocket = accept(_fds[0].fd, NULL, NULL);
    pollfd clientPoll;
    clientPoll.fd = clientSocket;
    clientPoll.events = POLLIN;
    _fds.push_back(clientPoll);
    Client nclient(clientSocket);
    _clients.push_back(nclient);
    std::cout << "new client has joined" << std::endl;
}
void Server::recieveData(int fd)
{
    char buffer[1024];
    int bytes = recv(fd, buffer, 1023, 0);
    if (bytes <= 0)
        return (clientDesconected(fd));
    buffer[bytes] = '\0';
    std::string message;
    if (searchClient(fd)->getNick() == "")
    {
        message = "client ";
        message += std::to_string(fd);
    }
    else
        message = searchClient(fd)->getNick();
    message += ": ";
    message += buffer;
    if (message.substr(10, 5) == "NICK ") // aqui pondras seguramente los comandos
    {
    //    message.pop_back();  peta pero hay que quitar el \n del final
        addNick(message.substr(15), fd);
        return ;
    }
    sendMessage(message, fd);
    std::cout << "client" << fd << ": " << buffer;
}
void Server::clientDesconected(int fd)
{
    for (int i = 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == fd)
        {
            close(fd);
            _fds.erase(_fds.begin() + i);
            std::cout << "client" << " desconected" << std::endl;
            return ;
        }
    }
}
void Server::sendMessage(std::string message, int fd)
{
    for (int i = 1; i < _fds.size(); i++)
    {
        if (_fds[i].fd != fd)
            send(_fds[i].fd, message.c_str(), message.size(), 0);
    }
}
void Server::addNick(std::string nick, int fd)
{
    Client *client = searchClient(fd);
    if (client == nullptr)
        return ;
    (*client).setNick(nick);
}
Client *Server::searchClient(int fd)
{
    for (int i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
            return (&_clients[i]);
    }
    std::cout << "no clients found" << std::endl;
    return (nullptr);
}