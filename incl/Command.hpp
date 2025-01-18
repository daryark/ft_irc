#pragma once

#include <iostream>
#include <vector>
#include <string>

enum  CommandType{
	USER = 1,
	NICK,
	PASS,
	JOIN,
	PRIVMSG,
	PING,
	PONG,
	QUIT,
	UNKNOWN
};

class Client;
class Command
{
private:
	std::string _command;
	std::vector<std::string> _params;
	Client * sender;

public:
	Command();
	Command(const Command &obj);
	Command &operator=(const Command &obj);
	~Command();

	// metchods
	void executeCommand();	
	void parse(const std::string& input, Client* sender);
	
	
	// static Command parse(const std::string& input, Client* sender);
	enum CommandType getTypeCommand();
};