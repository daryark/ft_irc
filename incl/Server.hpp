#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <sys/socket.h>  // socket, bind, listen, accept
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // htons
#include <poll.h>		 // pollfd

// class Channel;
// class Client;

// class Server
// {
// private:
// 	int _port;
// 	std::string _password;
// 	int _server_socket_fd; // used fo bind(), listen(), accept()
// 	struct sockaddr_in _addr;

// 	std::map<int, Client*> _clients; // int - fd_client and ptr to Client

// 	std::map<std::string, Channel*> _channels; // name_channel and ptr to Channel

// 	std::vector<pollfd> _pollfds; //
	
// 	bool _running; // флаг для остановки сервера.

// public:
// 	Server();
// 	Server(const Server &obj);
// 	Server &operator=(const Server &obj);
// 	~Server();

// 	// metchods
// 	void start();
// 	void stop();
// 	void handelNewConnection();

// 	// Channel *creatChannel(const std::string& name);
// 	Channel *creatChannel(std::string name);
// };