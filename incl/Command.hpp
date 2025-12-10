#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"

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
	void executePart(Client *client);
	void executeInvite(Client *client);
	void executeTopic(Client *client);
	void executeMode(Client *client);
	void executePong(Client *client);
	void executeQuit(Client *client);
	void executeCap(Client *client);

	bool isValidNickname()	const;
	void welcomeIfRegister(Client *client);
	bool checkPreconditions(Client* client, size_t in_args_size);
	void executePrivmsgToChannel(Client *client, const std::string& target);
	void executePrivmsgToClient(Client *client, const std::string& target);
	bool canKickFromChannel(Client* client, const std::string& channel_name);
	void executeKickOne(Client* client, const std::string& channel_name, const std::string& target_nick, const std::string& comment);
	void leaveChannels(Client* client, const std::set<std::string>& channels, const std::string msg);
	void leaveChannel(Client* client, Channel* channel, const std::string msg);

	//debug methods
	void executeAllClients(Client *client);
	void executeAllChannel(Client *client);
	void executeAllMembersInChannel(Client *client);
	void executeAllInfo(Client* client);

	void sendJoinInfo(Client *client, Channel *channel);
	const std::string formChannelMembersList(Channel *channel);
	Channel* joinNewChannel(Client *client, const std::string &channel_name, const std::string& pass);
	void joinExistingChannel(Client *client, Channel *channel, const std::string& pass);

public:
	Command(Server *server, const std::string &command, const std::vector<std::string> &args);
	~Command();
	// methods

	void executeCommand(Client *client);
};

void trim(std::string &s);


template <typename C>
C split(const std::string& input, char delimiter) {
    C tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter))
        tokens.insert(tokens.end(), token);
    return tokens;
}