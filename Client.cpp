#include "Client.hpp"

Client::Client()
{
    fd = 0;
    nickname = "*";
    username = "";
    realname = "";
    buffer = "";
    isOperator = 0;
    isPassSet = false;
    isNickSet = false;
    isUserSet = false;
    status = Client::UNREGISTERED;
}
Client::Client(int nfd)
{
    fd = nfd;
    nickname = "*";
    username = "";
    realname = "";
    buffer = "";
    isOperator = 0;
    isPassSet = false;
    isNickSet = false;
    isUserSet = false;
    status = Client::UNREGISTERED;
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
void Client::addBuffer(std::string buff)
{
    buffer += buff;
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
    if (!isNickSet) return "*";
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
 int Client::getStatus() const{
    return status;
 }
std::vector<std::string> Client::getChannels() 
{
    return (channels);
}
bool Client::getOperator() 
{
    return (isOperator);
}

bool Client::getPassSet(){
    return isPassSet;
}

bool Client::getUserSet(){
    return isUserSet;
}

bool Client::getNickSet(){
    return isNickSet;
}

void Client::setUserSet(){
    isUserSet = true;
}

void Client::setNickSet(){
    isNickSet = true;
}

bool Client::isAuth(){
    return Client::AUTHENTICATED == status;
}

bool Client::isUnReg(){
    return Client::UNREGISTERED == status;
}

void Client::setPass(){
    isPassSet = true;
}

void Client::setReg(){
    status = Client::REGISTERED;
}

void Client::setHostname(std::string hn){
    hostName = hn;
}

std::string Client::getHostname() const {
    return hostName;
}

std::string Client::getPrefix() {
    return ":" + getNick() + "!" + getUser() + "@" + getHostname() + " ";
}
void Client::sendMessage(int code, std::string message){
    std::ostringstream oss;
    oss << ":" << "ft_irc42" << " "
        << std::setfill('0') << std::setw(3) << code << " "
        << (nickname.empty() ? "*" : nickname) << " "
        << message << "\r\n";
    send(fd, oss.str().c_str(), oss.str().size(), 0); 
}