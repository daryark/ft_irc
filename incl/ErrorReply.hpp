#pragma once

#include "Server.hpp"

#define PREFIX(err_num)(std::string(":") + SERVER_NAME + " " + err_num + " ")

#define RPL_WELCOME(nick, user, host)(PREFIX("001") + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n")
#define PR_RUN		"Server is running on port"
#define PR_CLOSE	"Server closed"
#define PR_LISTEN	"Waiting for connections..."
#define PR_CL_NOT_CONNECT	"Can't connect the new client"
#define PR_CL_CONNECT		"New client connected on socket fd "
#define	PR_IN_MSG	"Register to join the server. Execute PASS, NICK, USER commands\r\n"
#define PR_USAGE	"Usage:\n"\
	"· KICK - Eject a client from the channel\n"\
	"· INVITE - Invite a client to a channel\n"\
	"· TOPIC - Change or view the channel topic\n"\
	"· MODE - Change the channel’s mode:\n"\
	"· 	i: Set/remove Invite-only channel\n"\
	"· 	t: Set/remove the restrictions of the TOPIC command to channel operators\n"\
	"· 	k: Set/remove the channel key (password)\n"\
	"· 	o: Give/take channel operator privilege\n"\
	"· 	l: Set/remove the user limit to channel\n"


// #define ERR_NORECIPIENT(nick)(PREFIX(411) + nick + " :No recipient given ( " + command + " )")

#define ERR_NONICKNAMEGIVEN(nick)(PREFIX("431") + nick + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(nick, new_nick)(PREFIX("432") + nick + new_nick + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(nick)(PREFIX("433") + nick +" :Nickname is already in use\r\n")

#define ERR_NOTREGISTERED(nick, command)(PREFIX("451") + nick + " " + command + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(nick, command)(PREFIX("461") + nick + " " + command + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(nick)(PREFIX("462") + nick + " :Unauthorized command (already registered)\r\n")
#define ERR_PASSWDMISMATCH(nick)(PREFIX("464") + nick + " :Password incorrect\r\n")