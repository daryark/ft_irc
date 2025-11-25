#pragma once

#include "Server.hpp"
#include "Command.hpp"

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

#define PR_RUN		"Server is running on port"
#define PR_CLOSE	"Server closed"
#define PR_LISTEN	"Waiting for connections..."
#define PR_CL_NOT_CONNECT	"Can't connect the new client"
#define PR_CL_CONNECT		"New client connected on socket fd "
	
#define MSG_PREFIX(nick, user, ip, command)(std::string(":") + nick + "!" + user + "@" + ip + " " + command + " ")
#define MSG(nick, user, ip, command, target, msg)(MSG_PREFIX(nick, user, ip, command) + target + " : " + msg + "\r\n")

#define PREFIX(rpl_code)(std::string(":") + SERVER_NAME + " " + rpl_code + " ")
#define RPL_WELCOME(nick, user, ip)(PREFIX("001") + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + ip + "\r\n")

#define RPL_NOTOPIC(channel)(PREFIX("331") + channel + " :No topic is set\r\n")
#define RPL_TOPIC(channel, topic)(PREFIX("332") + channel + " : " + topic + "\r\n")
#define RPL_NAMREPLY(nick, channel, names_list)(PREFIX("353") +  nick + " = " + channel + " : " + names_list + "\r\n")
#define RPL_ENDOFNAMES(nick, channel)(PREFIX("366") + nick + " " + channel + " :End of /NAMES list.\r\n")

#define ERR_NOSUCHNICK(nick)(PREFIX("401") + nick + " :No such nick\r\n")
#define ERR_NOSUCHCHANNEL(channel)(PREFIX("403") + channel + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(channel)(PREFIX("404") + channel + " :Cannot send to channel\r\n")

// #define ERR_NORECIPIENT(nick, command)(PREFIX("411") + nick + " :No recipient given (" + command + ")\r\n")
// #define ERR_NOTEXTTOSEND(nick)(PREFIX("412") + nick + " :No text to send\r\n")

#define ERR_UNKNOWNCOMMAND(command)(PREFIX("421") + command + " :Unknown command\r\n")
#define ERR_NONICKNAMEGIVEN(nick)(PREFIX("431") + nick + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(nick, new_nick)(PREFIX("432") + nick + " " + new_nick + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(nick)(PREFIX("433") + nick +" :Nickname is already in use\r\n")

#define ERR_USERNOTINCHANNEL(nick, channel)(PREFIX("441") + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(channel)(PREFIX("442") + channel + " :You're not on that channel\r\n")
#define ERR_USERONCHANNEL(nick, channel)(PREFIX("443") + nick + " " + channel + " :is already on channel\r\n")

#define ERR_NOTREGISTERED(nick, command)(PREFIX("451") + nick + " " + command + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(nick, command)(PREFIX("461") + nick + " " + command + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(nick)(PREFIX("462") + nick + " :Unauthorized command (already registered)\r\n")
#define ERR_PASSWDMISMATCH(name)(PREFIX("464") + name + " :Password incorrect\r\n") //name - nick/channel (server or channel pass mismatch)

#define ERR_CHANNELISFULL(channel)(PREFIX("471") + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(channel)(PREFIX("473") + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(channel)(PREFIX("475") + channel + " :Cannot join channel (+k)\r\n")
#define ERR_BADCHANNAME(channel)(PREFIX("479") + channel + ": Bad channel name\r\n")
#define ERR_CHANOPRIVSNEEDED(channel)(PREFIX("482") + channel + " :You're not channel operator\r\n")

#define RPL_CHANNELMODEIS(channel, modes)(PREFIX("324") + channel + " " + modes + " :is now channel modes\r\n")
#define ERR_UNKNOWNMODE(nick, mode)(PREFIX("472") + nick + " " + mode + " :is unknown mode\r\n")


#define RPL_INVITING(nick, target, channel)(PREFIX("341") + nick + " " + target + " " + channel + "\r\n")
