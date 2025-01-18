#include "../incl/Channel.hpp"

Channel::Channel()
{
	_name = "";
	_topic = "";
	_is_private = false;
	_password = "";
	_max_clients = -1;
	_is_invite_only = false;
}

Channel::Channel(std::string name)
{
	_name = name;
	_topic = "";
	_is_private = false;
	_password = "";
	_max_clients = -1;
	_is_invite_only = false;
}

Channel::Channel(const Channel &obj)
{
	if (this != &obj)
		*this = obj;
}

Channel &Channel::operator=(const Channel &copy)
{
	_name = copy._name;
	_topic = copy._topic;
	//vector
	// members = copy.members;
	// operators = copy.operators;
	_is_private = copy._is_private;
	_password = copy._password;
	_max_clients = copy._max_clients;
	_is_invite_only = copy._is_invite_only;
	return *this;
}

Channel::~Channel()
{
	members.clear();
	operators.clear();
}

void Channel::setTopic(const std::string& topic)
{
	_topic = topic;
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

void Channel::addClient(int client_fd)
{
}

void Channel::removeClient(int client_fd)
{
}

bool Channel::hasClient(int client_fd) const
{
	return false;
}

void Channel::addOperator(int client_fd)
{
}

void Channel::removeOperator(int client_fd)
{
}

bool Channel::isOperator(int client_fd) const
{
	return false;
}