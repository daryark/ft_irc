#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

class Client;

class Channel
{
private:
	std::string _name;
	std::string _topic;

	std::vector<Client *> _members;
	std::vector<Client *> _operators;

	std::string _password;

	int _max_clients;
	bool _is_invite_only;

public:
	Channel(const std::string &name);
	~Channel();

	// metchods
	void setName(const std::string &name); //+
	const std::string &getName() const;	   //+

	void setTopic(const std::string &topic); //+
	const std::string &getTopic() const;	 //+

	void setPassword(const std::string &password);	  //+
	bool checkKey(const std::string &password) const; //+

	void setInviteOnly(bool state); // +

	void addClient(Client *client);		 //+
	void removeClient(Client *client);	 //+
	bool isMember(Client *client) const; //+

	void addOperator(Client *client);	   //+
	void removeOperator(Client *client);   //+
	bool isOperator(Client *client) const; //+

	// void addClient(int client_fd);

	// void removeClient(int client_fd);

	//    void addOperator(int client_fd);

	// void removeOperator(int client_fd);

	// bool isOperator(int client_fd) const;

	// void broadcast(const std::string& message, int exclude_fd = -1) const;
};