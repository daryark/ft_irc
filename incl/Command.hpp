#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


#include "Client.hpp"
#include "Server.hpp"

enum  CommandType{
	UNKNOWN = 0,
	USER = 1,
	NICK,
	PASS,
	JOIN,
	PRIVMSG,
	//PONG,
	//QUIT,
	//PART,
	KICK,
	INVITE,
	TOPIC,
	MODE
};

class Client;
class Server;
class Command
{
private:
	typedef void (Command::*CommandHandler)(Client*, const std::vector<std::string>&);

	std::string _command;
	std::vector<std::string> _args;
	Server* _server;

	static std::map<std::string, CommandHandler> _commandMap;
	static void initCommandMap();

	void executeCommand(Client* client);



	void executeNick(Client* client);
	void executeUser(Client* client);
	void executePass(Client* client);
	void executeJoin(Client* client);
	void executePrivmsg(Client* client);
	void executeKick(Client* client);
	void executeInvite(Client* client);
	void executeTopic(Client* client);
	void executeMode(Client* client);

public:
	Command(Server* server);
	~Command();

	// metchods





	// static Command parse(const std::string& input, Client* sender);
};