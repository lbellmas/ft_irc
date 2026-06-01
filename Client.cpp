#include "Client.hpp"

Client::Client()
{
    fd = 0;
    nickname = "";
    username = "";
    realname = "";
    buffer = "";
    isOperator = 0;
}
Client::Client(int nfd)
{
    fd = nfd;
    nickname = "";
    username = "";
    realname = "";
    buffer = "";
    isOperator = 0;
}
Client::~Client() 
{

};
void Client::setFd(int nfd)
{
    fd = nfd;
}
void Client::setNick(std::string nick)
{
    nickname = nick;
}
void Client::setUser(std::string user)
{
    username = user;
}
void Client::setReal(std::string real)
{
    realname = real;
}
void Client::setBuffer(std::string buff)
{
    buffer = buff;
}
// void Client::setStatus(int status); este ns como va
void Client::setOperator(bool status)
{
    isOperator = status;
}

void Client::addChannels(std::string chann)
{
    channels.push_back(chann);
}

int Client::getFd() 
{
    return (fd);
}
std::string Client::getNick() 
{
    return (nickname);
}
std::string Client::getUser() 
{
    return (username);
}
std::string Client::getReal() 
{
    return (realname);
}
std::string Client::getBuff() 
{
    return (buffer);
}
// int Client::getStatus() const; ns como va esto
std::vector<std::string> Client::getChannels() 
{
    return (channels);
}
bool Client::getOperator() 
{
    return (isOperator);
}