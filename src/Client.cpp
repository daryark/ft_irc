#include "../incl/Client.hpp"

Client::Client(int fd, const struct sockaddr_in &addr): _fd(fd), _addr(addr){}

Client::~Client(){}

Client::Client(const Client& obj): _fd(obj._fd), _addr(obj._addr) {}

Client& Client::operator=(const Client& obj)
{
    if (this != &obj)
    {
        _fd = obj._fd;
        _addr = obj._addr;
        //!add copying of other props if add them in class constructor/header
    }
    return *this;
}