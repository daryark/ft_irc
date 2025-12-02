#include "../incl/Client.hpp"

Client::Client(const int fd,  const std::string& hostname, Server* server): _fd(fd), _hostname(hostname), _server(server)
{
	_nickname = "";
	_username = "";
	_servername = "";
	_realname = "";
	_authenticated = false;
	_registered = false;
	_incomplete_msg = "";
	_last_activity = time(NULL);
	_ping_sent = false;
}

Client::~Client()
{
	close(_fd);//?!it's already been closed on disconnection
	//_joined_channels.clear();
}

const int& Client::getFd() const { return _fd; }

std::deque<std::string>& Client::getMsgQueue() { return _msg_queue; };

void Client::setNickname(const std::string &nickname) { _nickname = nickname; }

const std::string &Client::getNickname() const { return _nickname; }

const std::string Client::getSafeNickname() const { return _nickname.empty() ? "*" : _nickname;}

void Client::setUsername(const std::string &username) { _username = username; }

const std::string &Client::getUsername() const { return _username; }

const std::string &Client::getHostname() const { return _hostname; }

const std::string &Client::getServername() const { return _servername; }

void Client::setRealname(const std::string &realname) { _realname = realname; }

const std::string &Client::getRealname() const { return _realname; }

void Client::authenticate(bool state) {	_authenticated = state; }

bool Client::isAuthenticated() const { return _authenticated; }

void Client::setRegistered(bool state) { _registered = state; }

bool Client::isRegistered() const { return _registered; }

time_t Client::getLastActivityTime() const { return _last_activity; }

void Client::setLastActivityTime(time_t time){ _last_activity = time; }

bool Client::isPingSent() const { return _ping_sent; }

void Client::setPingSent(bool status){ _ping_sent = status; }

std::string& Client::getIncompleteMsg(){ return _incomplete_msg; }

void Client::setIncompleteMsg(const std::string &incomplete_msg) { _incomplete_msg = incomplete_msg; }

void Client::setUser(const std::string& username, const std::string& realname){
	_username = username;
	_realname = realname;
	_servername = SERVER_NAME;
}

const std::set<std::string>& Client::getJoinedChannels() const { return _joined_channels; }

void Client::joinChannel(const std::string &channel){ _joined_channels.insert(channel); }

void Client::removeFromChannel(const std::string &channel){ _joined_channels.erase(channel); }

void Client::queueMsg(const std::string &message)
{
	_msg_queue.push_back(message);
	_server->markPfdForPollout(getFd());
}

void Client::printInfo() const
{
	// std::cout << "Client Info:" << std::endl;
	// std::cout << "FD: " << _fd << std::endl;
	// std::cout << "Nickname: " << _nickname << std::endl;
	// std::cout << "Username: " << _username << std::endl;
	// std::cout << "Hostname: " << _hostname << std::endl;
	// std::cout << "Servername: " << _servername << std::endl;
	// std::cout << "Realname: " << _realname << std::endl;
	// std::cout << "Authenticated: " << (_authenticated ? "Yes" : "No") << std::endl;
	// std::cout << "Registered: " << (_registered ? "Yes" : "No") << std::endl;
	std::cout << "Joined Channels: ";
	for (std::set<std::string>::iterator it = _joined_channels.begin(); it != _joined_channels.end(); ++it)
	{
		std::cout << *it;
		if (it != _joined_channels.end())
			std::cout << ", ";
	}
	std::cout << std::endl;
}