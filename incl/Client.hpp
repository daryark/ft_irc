#pragma once	

#include <iostream>
#include <vector>
#include <string>
#include <netinet/in.h>

class Client
{
private:
	const int _fd;
	const struct sockaddr_in _addr;
	// const std::string _ip;

	// std::string _nickname;

	// std::string _username;
	// std::string _hostname;
	// std::string _servername;
	// std::string _realname;

	// std::vector<std::string> joined_channels;
	// bool authenticated;

	Client &operator=(const Client &){return *this;};

public:
	Client(int fd, const struct sockaddr_in &addr);
	~Client();

	Client(const Client &obj);

	// metchods
	// void sendMessage(const std::string &message);

	// int getFd() const;
	
	// void setNickname(const std::string &nickname);
	// const std::string &getNickname() const;

	// void setUsername(const std::string &username);
	// const std::string &getUsername() const;

	// void authenticate(const std::string &password);
	// bool isAuthenticated() const;
	
	// void addChannel(const std::string &channel);
	// void removeChannel(const std::string &channel);

	// void setAddr(const struct sockaddr_in &addr);
	// const struct sockaddr_in &getAddr() const;
	
};