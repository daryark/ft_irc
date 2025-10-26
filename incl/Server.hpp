/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:09:46 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/26 18:57:03 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring> // memset
#include <vector>
#include <map>
#include <algorithm>

#include <csignal> //*9
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>	// htons
#include <netdb.h>		//NII_MAXHOST, NI_MAXSERV
#include <fcntl.h>		//fnctl - O_NONBLOCK mode
#include <poll.h>		// pollfd
#include <cstdlib>		//exit
#include <unistd.h>		//close

#include <cerrno>

#include <stdio.h>

#include "../incl/colors.hpp"
// class CommandFactory;
class Command;
  class Channel;
  class Client;

#include "Channel.hpp"

#include "../incl/Client.hpp"

#define MAX_MSG	512

//# CREATE ERR CODES FILE
#define PR_RUN		"Server is running on port"
#define PR_CLOSE	"Server closed"
#define PR_LISTEN	"Waiting for connections..."
#define PR_CL_NOT_CONNECT	"Can't connect the new client"
#define PR_CL_CONNECT		"New client connected on socket fd "
#define PR_WELCOME	" :Welcome to the IRC server"
#define	PR_IN_MSG	"Register to join the server. Execute PASS, NICK, USER commands"
#define PR_USAGE	"Usage:\n"\
	"· KICK - Eject a client from the channel\n"\
	"· INVITE - Invite a client to a channel\n"\
	"· TOPIC - Change or view the channel topic\n"\
	"· MODE - Change the channel’s mode:\n"\
	"· 	i: Set/remove Invite-only channel\n"\
	"· 	t: Set/remove the restrictions of the TOPIC command to channel operators\n"\
	"· 	k: Set/remove the channel key (password)\n"\
	"· 	o: Give/take channel operator privilege\n"\
	"· 	l: Set/remove the user limit to channel\n"\

class Server
{
private:
	int						_head_socket; // used fo bind(), listen(), accept()
	int 					_port;
	std::string 			_password;
	struct sockaddr_in		_addr;
	std::vector<pollfd> 	_pollfds;
	std::map<int, Client*>	_clients; // int - fd_client and ptr to Client
	std::map<std::string, Channel*> _channels; // name_channel and ptr to Channel

	Server(const Server &){};
	Server &operator=(const Server &){return *this;};
	//helpers //#REWRITE to CamelCase!!!!
	void					accept_client();
	void					read_msg(int fd);
	void					push_pollfd(int, short, short);
	void					process_in_msg(int fd, char* buf, unsigned int len);
	void					fill_sockaddr_in(struct sockaddr_in& addr, short int in_family, unsigned short int in_port ,uint32_t s_addr);
	void					fancy_print(const std::string& opt);
	
	public:
	Server();
	Server(int port, std::string password);
	~Server();
	
	// methods
	void					init();
	void					run();
	void					disconnect_client(int fd);
	
	void					markPfdForPollout(int fd);
	void					send_msg(int fd);
	void					send_color(int fd, const std::string& msg, const std::string& color = RE);

	// void handelNewConnection();

	// Channel *creatChannel(const std::string& name);
	// Channel *creatChannel(std::string name);

	const std::string &getPassword() const; //+
	const std::map<int, Client*> &getClients() const; //+
	const std::map<std::string, Channel*> &getChannel() const;//+

	Client*	getClient(int fd)	const;

	Channel* getChannelByName(const std::string& name);//+

	Channel* createChannel(const std::string& nameChannel); //+

	Client* getClientByNickname(const std::string& nickname); //-

	// void handleInput(Client* client, const std::string& input) //*parser input + masha check CommandFactory.hpp
};