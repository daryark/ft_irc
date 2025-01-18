#include "../incl/Client.hpp"

Client::Client()
{
	_fd = -1;
	_ip = "";
	_nickname = "";
	_username = "";
	_hostname = "";
	_servername = "";
	_realname = "";
	authenticated = false;
}

// Client::Client(int fd, const struct sockaddr_in &addr)
// {
// 	_fd = fd;
// 	_addr = addr;
// 	// _ip = inet_ntoa(addr.sin_addr);
// 	_nickname = "";
// 	_username = "";
// 	_hostname = "";
// 	_servername = "";
// 	_realname = "";
// 	authenticated = false;
// }

Client::Client(int fd, std::string nickname, std::string username, std::string hostname, std::string servername, std::string realname)
{
	_fd = fd;
	_ip = "";
	_nickname = nickname;
	_username = username;
	_hostname = hostname;
	_servername = servername;
	_realname = realname;
	authenticated = false;
}

Client::Client(const Client &obj)
{
	if (this != &obj)
		*this = obj;
}

Client &Client::operator=(const Client &copy)
{
	_fd = copy._fd;
	_addr = copy._addr;
	_ip = copy._ip;
	_nickname = copy._nickname;
	_username = copy._username;
	_hostname = copy._hostname;
	_servername = copy._servername;
	_realname = copy._realname;
	authenticated = copy.authenticated;
	return *this;
}

Client::~Client()
{
	joined_channels.clear();
}

void Client::sendMessage(const std::string &message)
{
	Command::parse(message, this).executeCommand();
	// send(_fd, message.c_str(), message.size(), 0);
}

int Client::getFd() const
{
	return _fd;
}

void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

const std::string &Client::getNickname() const
{
	return _nickname;
}

void Client::setUsername(const std::string &username)
{
	_username = username;
}

const std::string &Client::getUsername() const
{
	return _username;
}

void Client::authenticate(const std::string &password)
{
	// authenticated = true;
}

bool Client::isAuthenticated() const
{
	return authenticated;
}

void Client::addChannel(const std::string &channel)
{
	joined_channels.push_back(channel);
}

void Client::removeChannel(const std::string &channel)
{
	std::vector<std::string>::iterator it = std::find(joined_channels.begin(), joined_channels.end(), channel);
	if (it != joined_channels.end()) {
        joined_channels.erase(it);
    }
	std::cout << "Client::removeChannel" << std::endl;
}

void Client::setAddr(const struct sockaddr_in &addr)
{
	_addr = addr;
}

const struct sockaddr_in &Client::getAddr() const
{
	return _addr;
}

