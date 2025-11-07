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

	//vector
	// std::vector<Client *> _members;
	// std::vector<Client *> _operators;
	// std::vector<Client *> _invitedClient;

	std::string _password;
	bool _hasPassword;

	int _max_clients;
	bool _is_invite_only;

public:
	Channel(const std::string &name, const std::string &password);
	~Channel();

	// methods
	void setName(const std::string &name); //+
	const std::string &getName() const;	   //+

	void setTopic(const std::string &topic); //+
	const std::string &getTopic() const;	 //+

	void setPassword(const std::string &password);	  //+
	bool checkPasswordEquality(const std::string &password) const; //+

	bool hasPassword() const;//+

	void setInviteOnly(bool state); // +
	bool isInviteOnly() const;
	bool isInvitedClient(Client* client) const;

	void addClient(Client *client);		 //+
	void removeClient(Client *client);	 //+
	bool isMember(Client *client) const; //+
	const std::set<Client *>::const_iterator getMembersBegin() const;

	void addOperator(Client *client);	   //+
	void removeOperator(Client *client);   //+
	bool isOperator(Client *client) const; //+
	bool hasOperator() const; //+

	bool isFull() const;//+

	void globalMessage(Client* sender, std::string message) const; //+

	int getSize() const; //+

	std::set<Client *> getClients();

	// void addClient(int client_fd);

	// void removeClient(int client_fd);

	//    void addOperator(int client_fd);

	// void removeOperator(int client_fd);

	// bool isOperator(int client_fd) const;

	// void broadcast(const std::string& message, int exclude_fd = -1) const;
};