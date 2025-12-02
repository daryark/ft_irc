#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <sys/socket.h>

#include "../incl/Client.hpp"

class Client;
class Channel
{
private:
	std::string _name;
	std::string _topic;

	std::set<Client *> _members;
	std::set<Client *> _operators;
	std::set<Client *> _invitedClient;

	std::string _password;
	bool _hasPassword;

	// new state
	int _max_clients;
	bool _hasLimit; // #into snake case

	bool _isTopicSetByOperator; // #into snake case
	bool _is_invite_only;

public:
	Channel(const std::string &name, const std::string &password);
	~Channel();

	// methods
	void setName(const std::string &name); //+
	const std::string &getName() const;	   //+

	void setTopic(const std::string &topic); //+
	const std::string &getTopic() const;	 //+

	void setPassword(const std::string &password); //+
	bool checkPasswordEquality(const std::string &password) const; //+

	bool hasPassword() const; //+

	void setInviteOnly(bool state); // +
	bool isInviteOnly() const;
	bool isInvitedClient(Client *client) const;
	void addInvitedClient(Client *client);
	void removeInvitedClient(Client *client); //+

	void addClient(Client *client);		 //+
	void removeClient(Client *client);	 //+
	bool isMember(Client *client) const; //+

	void addOperator(Client *client);	   //+
	void removeOperator(Client *client);   //+
	bool isOperator(Client *client) const; //+
	bool hasOperator() const;			   //+
	bool isFull() const;				   //+

	void globalMessage(Client *sender, std::string message, bool send_to_sender) const; //+

	int getSize() const;			 //+
	std::set<Client *> getClients(); //+
	std::set<Client *> getOperators();
	std::set<Client *> getInvitedClients();

	//	new methods
	bool isTopicSetByOperator() const;		//+
	void setTopicSetByOperator(bool state); //+

	bool hasLimit() const;				 //+
	void setMaxClients(int max_clients); //+

	const std::string &getPassword() const;
	void deletePassword();
	void deleteLimits();
};