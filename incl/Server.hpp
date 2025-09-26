/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:09:46 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/09/26 17:53:00 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>       // memset
#include <vector>
#include <map>
#include <algorithm>

#include <sys/socket.h>  // socket, bind, listen, accept
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // htons
#include <netdb.h>		 //NII_MAXHOST, NI_MAXSERV
#include <fcntl.h>		 //fnctl - O_NONBLOCK mode
#include <poll.h>		 // pollfd
#include <cstdlib>       //exit
#include <unistd.h>      //close

#include <cerrno>

#include "../incl/colors.hpp"
#include "../incl/Client.hpp"

// class Channel;
// class Client;

#define MAX_MSG	512

#define PR_RUN		"Server is running on port"
#define PR_CLOSE	"Server closed"
#define PR_LISTEN	"Waiting for connections..."
#define PR_CL_NOT_CONNECT	"Can't connect the new client"
#define PR_CL_CONNECT		"New client connected on socket fd "
#define PR_WELCOME	"Welcome on the server"
#define PR_USAGE	"Usage:\n"\
	"· KICK - Eject a client from the channel\n"\
	"· INVITE - Invite a client to a channel/n"\
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
	std::map<int, Client>	_clients; // int - fd_client and ptr to Client

	// std::map<std::string, Channel*> _channels; // name_channel and ptr to Channel

	Server(const Server &){};
	Server &operator=(const Server &){return *this;};
	//helpers
	void					accept_client();
	void					disconnect_client(int fd);
	void					read_msg(int fd);
	void					push_pollfd(int, short, short);
	void					process_msg(int fd, char* buf);

	void					fancy_print(const std::string& opt);
	void					send_color(int fd, const std::string& msg, const std::string& color = RE);

public:
	Server(int port, std::string password);
	~Server();

	// methods
	void					init();
	void					run();

	// void handelNewConnection();

	// Channel *creatChannel(const std::string& name);
	// Channel *creatChannel(std::string name);
};