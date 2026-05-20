#include "server.hpp"

void server::init()
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
void server::run()
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
        if (_fds.size() == 1) // cunado no ha adie coectado apaga
            break ;
    }
}
void server::acceptClient()
{
    int clientSocket = accept(_fds[0].fd, NULL, NULL);
    pollfd clientPoll;
    clientPoll.fd = clientSocket;
    clientPoll.events = POLLIN;
    _fds.push_back(clientPoll);
    std::cout << "new client has joined" << std::endl;
}
void server::recieveData(int fd)
{
    char buffer[1024];
    int bytes = recv(fd, buffer, 1023, 0);
    if (bytes <= 0)
        return (clientDesconected(fd));
    buffer[bytes] = '\0';
    std::string message = "client ";
    message += std::to_string(fd);
    message += buffer;
    sendMessage(message, fd);
    std::cout << "client" << fd << ": " << buffer;
}
void server::clientDesconected(int fd)
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
void server::sendMessage(std::string message, int fd)
{
    for (int i = 1; i < _fds.size(); i++)
    {
        if (_fds[i].fd != fd)
            send(_fds[i].fd, message.c_str(), message.size(), 0);
    }
}