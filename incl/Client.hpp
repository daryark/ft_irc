#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <netinet/in.h>

#include "../incl/Command.hpp"

// class Command;
class Client
{
private:
	int _fd;
	const struct sockaddr_in _addr;
	// const int _fd;//#const or not?
	// struct sockaddr_in _addr; //#const or not?

	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _servername;
	std::string _realname;

	bool _authenticated;
	bool _registered;

	std::vector<std::string> _joined_channels;

public:
	Client(int fd, sockaddr_in addr);
	// Client(int fd, std::string nickname, std::string username,
	// 	   std::string hostname, std::string servername, std::string realname);
	// Client(int fd, std::string username, std::string hostname,
		//    std::string servername, std::string realname);

	~Client();

	Client(const Client &obj);

	// methods
	// void sendMessage(const std::string &message) const;

	int &getFd(); //+

	void setUser(const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname);
	void setUserDefault(const std::string& username, const std::string& realname);

	void setNickname(const std::string &nickname); //+
	const std::string &getNickname() const;		   //+

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

	void joinChannel(const std::string &channel);	//+
	void removeChannel(const std::string &channel); //+

	void printInfo() const; //+

	// void setAddr(const struct sockaddr_in &addr);
	// const struct sockaddr_in &getAddr() const;
};