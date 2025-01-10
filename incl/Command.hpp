#pragma once

#include <iostream>
#include <vector>
#include <string>

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
	static Command parse(const std::string& input, Client* sender);
};