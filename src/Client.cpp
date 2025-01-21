#include "../incl/Client.hpp"

Client::Client(int fd, const struct sockaddr_in &addr): _fd(fd), _addr(addr){}

Client::~Client(){}

Client::Client(const Client& obj): _fd(obj._fd), _addr(obj._addr) {}