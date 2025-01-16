/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:09:46 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/01/15 21:19:07 by dyarkovs         ###   ########.fr       */
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
#include <poll.h>		 // pollfd
#include <netdb.h>		 //NII_MAXHOST, NI_MAXSERV
#include <cstdlib>       //exit
#include <unistd.h>      //close

#include "../incl/colors.hpp"
// class Channel;
// class Client;

class Server
{
private:
	int					_head_socket; // used fo bind(), listen(), accept()
	int 				_port;
	std::string 		_password;
	struct sockaddr_in	_addr;

	// std::map<int, Client*> _clients; // int - fd_client and ptr to Client
	// std::map<std::string, Channel*> _channels; // name_channel and ptr to Channel
	// std::vector<pollfd> _pollfds; //
	// bool _running; // флаг для остановки сервера.

	Server &operator=(const Server &obj){};
	Server(const Server &obj){return *this};

public:
	Server(int port, std::string password);
	~Server();

	// metchods
	void	init();
	// void start();
	// void stop();
	// void handelNewConnection();

	// Channel *creatChannel(const std::string& name);
	// Channel *creatChannel(std::string name);
};