#include "../incl/Channel.hpp"

Channel::Channel(const std::string &name, const std::string &password): _name(name), _password(password)
{
	_topic = "Default";
	_max_clients = -1;
  _hasPassword = (password.empty() ? false : true);
	_is_invite_only = false;
  _hasLimit = false;
  _isTopicSetByOperator = false;
}

Channel::~Channel()
{
	_members.clear();
	_operators.clear();
  _invitedClient.clear();
}

void Channel::setName(const std::string &name) { _name = name; }

const std::string &Channel::getName() const { return _name; }

void Channel::setTopic(const std::string &topic) { _topic = topic; }

const std::string &Channel::getTopic() const { return _topic; }

void Channel::setPassword(const std::string &password) 
{ 
  _password = password;
  _hasPassword = true;
}

bool Channel::checkPasswordEquality(const std::string &password) const 
{ 
  return (password == _password || _password.empty()) ? true : false;
}

bool Channel::hasPassword() const { return _hasPassword; }

void Channel::setInviteOnly(bool state)
{
	if (_is_invite_only == state)
		std::cout << "this state: " << state << "has already been set for the channel!" << std::endl;
	else
		_is_invite_only = state;
}

bool Channel::isInviteOnly() const { return _is_invite_only; }

bool Channel::isInvitedClient(Client *client) const {

  return _invitedClient.find(client) != _invitedClient.end();
  //vector
  //  return std::find(_invitedClient.begin(), _invitedClient.end(), client) != _invitedClient.end();
}

void Channel::addClient(Client *client)
{
  //set
  _members.insert(client);
  //vector
//	_members.push_back(client);
}

void Channel::removeClient(Client *client)
{
  if (_members.find(client) != _members.end()){
    _members.erase(client);
    _operators.erase(client);
  }
    //vector
//	_members.erase(std::remove(_members.begin(), _members.end(), client), _members.end());
//	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isMember(Client *client) const
{
	return std::find(_members.begin(), _members.end(), client) != _members.end();
}

void Channel::addOperator(Client *client)
{
  //
  _operators.insert(client);
  _members.insert(client);

  //vector
//	if (std::find(_operators.begin(), _operators.end(), client) == _operators.end())
//	{
//		_operators.push_back(client);
//	}
}

void Channel::removeOperator(Client *client)
{
  //set
  _operators.erase(client);
  //vector
  // _operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isOperator(Client *client) const
{
  //set
	return _operators.find(client) != _operators.end();
  //vector
  //return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

bool Channel::hasOperator() const { return !_operators.empty(); }

bool Channel::isFull() const{ return (int)_members.size() >= _max_clients; }

//need to improve send to all members
void Channel::globalMessage(Client* sender, std::string message, bool send_to_sender) const
{
  for (std::set<Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
  {
    if (*it == sender && !send_to_sender)
      continue;
    (*it)->queueMsg(message);
  }
}

int Channel::getSize() const { return _members.size(); }

std::set<Client *> Channel::getClients(){return _members; }
std::set<Client *> Channel::getOperators(){return _operators; }
std::set<Client *> Channel::getInvitedClients(){return _invitedClient; }


//new methods
bool Channel::isTopicSetByOperator() const { return _isTopicSetByOperator; }  
void Channel::setTopicSetByOperator(bool state) { _isTopicSetByOperator = state; }

bool Channel::hasLimit() const { return _hasLimit; }
void Channel::setMaxClients(int max_clients)
{
  _max_clients = max_clients;
  _hasLimit = true;
}


void Channel::addInvitedClient(Client *client){_invitedClient.insert(client);}
void Channel::removeInvitedClient(Client *client) {_invitedClient.erase(client);}

const std::string &Channel::getPassword() const{return _password;}

void Channel::deletePassword() 
{ 
  _password = "";
  _hasPassword = false;
}

void Channel::deleteLimits() 
{ 
  _max_clients = -1;
  _hasLimit = false;
}
  