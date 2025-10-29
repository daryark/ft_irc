#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring> // memset
#include <csignal> //*9
#include <cerrno>
#include <cstdlib>		//exit
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>	// htons
#include <netdb.h>		//NII_MAXHOST, NI_MAXSERV
#include <fcntl.h>		//fnctl - O_NONBLOCK mode
#include <poll.h>		// pollfd
#include <unistd.h>		//close
#include <stdio.h>

#include "../incl/Client.hpp"
#include "../incl/ErrorReply.hpp"
#include "../incl/colors.hpp"
class Command;
class Channel;
class Client;

#define SERVER_NAME "ircserv"
#define MAX_MSG	512

extern volatile sig_atomic_t g_runnning;//*9

class Server
{
private:
	int _head_socket;
	int _port;
	std::string _password;
	struct sockaddr_in _addr;
	std::vector<pollfd> _pollfds;
	std::map<int, Client*> _clients; // int - fd_client and ptr to Client
	std::map<std::string, Channel*> _channels; // name_channel and ptr to Channel

	Server(const Server &){};
	Server &operator=(const Server &){return *this;};
	//helpers //#REWRITE to CamelCase!!!!
	void							acceptClient();
	int								readMsg(int fd);
	void							pushPollfd(int, short, short);
	void							processInMsg(int fd, char* buf, unsigned int len);
	void							fillSockaddrIn(struct sockaddr_in& addr, short int in_family, unsigned short int in_port ,uint32_t s_addr);
	void							fancyPrint(const std::string& opt);
	
	public:
	Server();
	Server(int port, std::string password);
	~Server();
	
	// methods
	void							init();
	void							run();
	// std::vector<pollfd>::iterator	disconnect_client(int fd);
	void							disconnectClient(int fd);
	
	void							markPfdForPollout(int fd);
	void							sendMsg(int fd);
	// void							send_color(int fd, const std::string& msg, const std::string& color = RE);

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