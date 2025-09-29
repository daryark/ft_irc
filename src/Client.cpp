#include "../incl/Client.hpp"

Client::Client(int fd,  sockaddr_in addr): _fd(fd), _addr(addr)
{
	_nickname = "";
	_username = "";
	_hostname = "";
	_servername = "";
	_realname = "";
	_authenticated = false;
	_registered = false;
}

// Client::Client(int fd, std::string nickname, std::string username, std::string hostname, std::string servername, std::string realname)
// {
// 	_fd = fd;
// 	_nickname = nickname;
// 	_username = username;
// 	_hostname = hostname;
// 	_servername = servername;
// 	_realname = realname;
// 	_authenticated = false;
// 	_registered = false;
// }

Client::~Client()
{
	close(_fd);
	//_joined_channels.clear();
}

// void Client::sendMessage(const std::string &message) const
// {
// 	// Command::parse(message, this).executeCommand();
// 	// send(_fd, message.c_str(), message.size(), 0);
// }

int &Client::getFd() { return _fd; }

void Client::setNickname(const std::string &nickname) { _nickname = nickname; }

const std::string &Client::getNickname() const { return _nickname; }

void Client::setUsername(const std::string &username) { _username = username; }

const std::string &Client::getUsername() const { return _username; }

void Client::setHostname(const std::string &hostname) { _hostname = hostname; }

const std::string &Client::getHostname() const { return _hostname; }

void Client::setServername(const std::string &servername) { _servername = servername; }

const std::string &Client::getServername() const { return _servername; }

void Client::setRealname(const std::string &realname) { _realname = realname; }

const std::string &Client::getRealname() const { return _realname; }

void Client::authenticate(bool state)
{
	_authenticated = state;
	// authenticated = true;
}

bool Client::isAuthenticated() const
{
	return _authenticated;
}

void Client::setRegistered(bool state)
{
	_authenticated = state;
	// authenticated = true;
}

bool Client::isRegistered() const
{
	return _authenticated;
}

void Client::setUser(const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname){
  _username = username;
  _hostname = hostname;
  _servername = servername;
  _realname = realname;
}

void Client::setUserDefault(const std::string& username, const std::string& realname){
  _username = username;
  _realname = realname;
  _hostname = "0";
  _servername = "*";
}



void Client::joinChannel(const std::string &channel)
{
	_joined_channels.push_back(channel);
}

void Client::removeChannel(const std::string &channel)
{
	/*std::vector<std::string>::iterator it = std::find(joined_channels.begin(), joined_channels.end(), channel);
	if (it != joined_channels.end()) {
		joined_channels.erase(it);
	}
	std::cout << "Client::removeChannel" << std::endl;*/

	_joined_channels.erase(std::remove(_joined_channels.begin(), _joined_channels.end(), channel), _joined_channels.end());
}

/*void Client::setAddr(const struct sockaddr_in &addr)
{
	_addr = addr;
}

const struct sockaddr_in &Client::getAddr() const
{
	return _addr;
}*/
