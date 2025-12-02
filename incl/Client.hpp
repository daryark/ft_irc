#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <set>
#include <string>
#include <algorithm>
#include <netinet/in.h>
#include <ctime>

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
	
	time_t _last_activity;
	bool _ping_sent;

	std::string _incomplete_msg;
	std::deque<std::string> _msg_queue;
	std::set<std::string> _joined_channels;

public:
	Client(const int fd, const std::string& hostname, Server* server);
	~Client();

	Client(const Client &obj);

	const int& getFd() const; //+
	std::deque<std::string>& getMsgQueue();
	void queueMsg(const std::string &message);
	std::string& getIncompleteMsg();		   //+
	void setIncompleteMsg(const std::string &incomplete_msg); //+

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

	time_t getLastActivityTime() const;
	void setLastActivityTime(time_t time);

	bool isPingSent() const;
	void setPingSent(bool status);
	void updateActive();

	void joinChannel(const std::string &channel);	//+
	void removeFromChannel(const std::string &channel); //+
	const std::set<std::string>& getJoinedChannels() const ;

	void printInfo() const; //+
};