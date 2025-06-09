#include "../incl/Channel.hpp"

Channel::Channel(const std::string &name)
{
	_name = name;
	_topic = "unkmown";
	//_is_private = false;
	_password = "";
	_max_clients = -1;
	_is_invite_only = false;
}

Channel::~Channel()
{
	_members.clear();
	_operators.clear();
}

void Channel::setName(const std::string &name) { _name = name; }

const std::string &Channel::getName() const { return _name; }

void Channel::setTopic(const std::string &topic) { _topic = topic; }

const std::string &Channel::getTopic() const { return _topic; }

void Channel::setPassword(const std::string &password) { _password = password; }

bool Channel::checkKey(const std::string &password) const
{
	return (password == _password || _password.empty()) ? true : false;
}

void Channel::setInviteOnly(bool state)
{
	if (_is_invite_only == state)
		std::cout << "this state: " << state << "has already been set for the channel!" << std::endl;
	else
		_is_invite_only = state;
}

void Channel::addClient(Client *client)
{
	_members.push_back(client);
}

void Channel::removeClient(Client *client)
{
	_members.erase(std::remove(_members.begin(), _members.end(), client), _members.end());
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isMember(Client *client) const
{
	return std::find(_members.begin(), _members.end(), client) != _members.end();
}

void Channel::addOperator(Client *client)
{
	if (std::find(_operators.begin(), _operators.end(), client) == _operators.end())
	{
		_operators.push_back(client);
	}
}

void Channel::removeOperator(Client *client)
{
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isOperator(Client *client) const
{
	return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}