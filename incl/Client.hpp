#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <netinet/in.h>
#include <deque>

#include "../incl/Server.hpp"
#include "../incl/Command.hpp"

// class Command;
class Client
{
private:
	int _fd;
	const struct sockaddr_in _addr;
	Server*	_server;

	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _servername;
	std::string _realname;

	bool _authenticated; //pass
	bool _registered;

	std::string _incomplete_msg;
	std::deque<std::string> _msg_queue;
	std::vector<std::string> _joined_channels;

public:
	Client(int fd, sockaddr_in addr, Server* server);
	// Client(int fd, std::string nickname, std::string username,
	// 	   std::string hostname, std::string servername, std::string realname);
	// Client(int fd, std::string username, std::string hostname,
		//    std::string servername, std::string realname);

	~Client();

	Client(const Client &obj);

	// methods
	// void sendMessage(const std::string &message) const;

	int& getFd(); //+
	std::deque<std::string>& getMsgQueue();

	void setUser(const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname);
	void setUserDefault(const std::string& username, const std::string& realname);

	void setNickname(const std::string &nickname); //+
	const std::string &getNickname() const;		   //+
	const std::string getSafeNickname() const;

	void setUsername(const std::string &username); //+
	const std::string &getUsername() const;		   //+

	void setHostname(const std::string &hostname); //+
	const std::string &getHostname() const;		   //+

	void setServername(const std::string &servername); //+
	const std::string &getServername() const;		   //+

	void setRealname(const std::string &realname); //+
	const std::string &getRealname() const;		   //+

	void authenticate(bool state); //+
	bool isAuthenticated() const;  //+

	void setRegistered(bool state); //+
	bool isRegistered() const;  //+

	void setIncompleteMsg(const std::string &incomplete_msg); //+
	std::string& getIncompleteMsg();		   //+

	void joinChannel(const std::string &channel);	//+
	void removeChannel(const std::string &channel); //+

	void queueMsg(const std::string &message);
	void printInfo() const; //+

	// void setAddr(const struct sockaddr_in &addr);
	// const struct sockaddr_in &getAddr() const;
};