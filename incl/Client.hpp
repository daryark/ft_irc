#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <netinet/in.h>
#include <deque>
#include <set>

#include "../incl/Server.hpp"
#include "../incl/Command.hpp"

// class Command;
class Client
{
private:
	const int _fd;
	const std::string _hostname;
	Server*	_server;

	std::string _nickname;
	std::string _username;
	std::string _servername;
	std::string _realname;

	bool _authenticated; //pass
	bool _registered;

	std::string _incomplete_msg;
	std::deque<std::string> _msg_queue;
	std::set<std::string> _joined_channels;

public:
	Client(const int fd, const std::string& hostname, Server* server);
	~Client();

	Client(const Client &obj);

	// methods
	// void sendMessage(const std::string &message) const;

	const int& getFd() const; //+
	std::deque<std::string>& getMsgQueue();
	void queueMsg(const std::string &message);
	void setIncompleteMsg(const std::string &incomplete_msg); //+
	std::string& getIncompleteMsg();		   //+

	void setUser(const std::string& username, const std::string& realname);

	void setNickname(const std::string &nickname); //+
	const std::string &getNickname() const;		   //+
	const std::string getSafeNickname() const;

	void setUsername(const std::string &username); //+
	const std::string &getUsername() const;		   //+

	const std::string &getHostname() const;		   //+

	const std::string &getServername() const;		//+

	void setRealname(const std::string &realname); //+
	const std::string &getRealname() const;		   //+

	void authenticate(bool state); //+
	bool isAuthenticated() const;  //+

	void setRegistered(bool state); //+
	bool isRegistered() const;  //+


	void joinChannel(const std::string &channel);	//+
	void removeFromChannel(const std::string &channel); //+
	const std::set<std::string>& getJoinedChannels() const ;

	void printInfo() const; //+
};