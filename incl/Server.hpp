/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mperetia <mperetia@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:09:46 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/01/19 22:39:03 by mperetia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstring>       // memset
#include <vector>
#include <map>

#include <sys/socket.h>  // socket, bind, listen, accept
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // htons
#include <netdb.h>		 //NII_MAXHOST, NI_MAXSERV
#include <fcntl.h>		 //fnctl - O_NONBLOCK mode
#include <poll.h>		 // pollfd
#include <cstdlib>       //exit
#include <unistd.h>      //close

#include "../incl/colors.hpp"
//class CommandFactory;
//class Command;
// class Channel;
// class Client;

#define PR_RUN		1
#define PR_CLOSE	2
#define PR_LISTEN	3

class Server
{
private:
	int					_head_socket; // used fo bind(), listen(), accept()
	int 				_port;
	std::string 		_password;
	struct sockaddr_in	_addr;

	// std::map<int, Client*> _clients; // int - fd_client and ptr to Client
	// std::map<std::string, Channel*> _channels; // name_channel and ptr to Channel
	std::vector<pollfd> _pollfds;

	Server(const Server &obj){(void)obj;};
	Server &operator=(const Server &obj){(void)obj; return *this;};
	void				fancy_print(int opt);

public:
	Server();
	Server(int port, std::string password);
	~Server();

	// methods
	void	init();
	void	run();
	// void handelNewConnection();

	// Channel *creatChannel(const std::string& name);
	// Channel *creatChannel(std::string name);

	//void handleInput(Client* client, const std::string& input) //*parser input + masha
};