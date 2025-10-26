#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"


enum CommandType
{
	UNKNOWN = 0,
	USER = 1,
	NICK,
	PASS,
	JOIN,
	PRIVMSG,
	QUIT,
	// PONG,
	// PART,
	KICK,
	INVITE,
	TOPIC,
	MODE
};
class Channel;
class Client;
class Server;
class Command
{
private:
	typedef void (Command::*CommandHandler)(Client *);

	std::string _command;
	std::vector<std::string> _args;

	Server *_server;

	static std::map<std::string, CommandHandler> _commandMap;
	static void initCommandMap();

	void executeNick(Client *client);//+
	void executeUser(Client *client);//+
	void executePass(Client *client); //+
	void executeJoin(Client *client);//+
	void executePrivmsg(Client *client);
	void executeKick(Client *client);
	void executeInvite(Client *client);
	void executeTopic(Client *client);
	void executeMode(Client *client);
	void executeQuit(Client *client);

	bool isValidNickname()	const;


	void executeAllClients(Client* client);
	void executeAllChannel(Client* client);
	void executeAllMembersInChannel(Client* client);
	void executeAllInfo(Client* client);

public:
	Command(Server *server, const std::string &command, const std::vector<std::string> &args);
	~Command();
	// metchods

	void executeCommand(Client *client);
	
	void PrintAllClients( Server& server);
	void PrintAllChannels( Server& server);
	void PrintMembersInChannel( Server& server, const std::string& channelName);
};

