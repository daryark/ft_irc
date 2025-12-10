#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring> // memset
#include <csignal> //*9
#include <cerrno>
#include <cstdlib>		//exit
#include <ctime>
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
#define PING_INTERVAL 60 //60
#define PONG_TIMEOUT 30 //30

extern volatile sig_atomic_t g_runnning;//*9

class Server
{
private:
	int _head_socket;
	int _port;
	std::string _password;

	struct sockaddr_in _addr;
	std::vector<pollfd> _pollfds;

	std::map<int, Client*> _clients; // int - clients fd
	std::map<std::string, Channel*> _channels; // str - channel_name

	
	Server(const Server &){};
	Server &operator=(const Server &){return *this; };

	bool actionOnFd(pollfd& pollfd);
	void acceptClient();
	bool readMsg(int fd);
	void sendMsg(pollfd& pollfd);
	void processInMsg(int fd, char* buf, int len);
	bool cleanClient(int fd);
	//helpers
	void fillSockaddrIn(struct sockaddr_in& addr, short int in_family, unsigned short int in_port ,uint32_t s_addr);
	void pushPollfd(int, short, short);
	void setSocketNonBlock(int fd);
	void checkClientsTimeouts();
	void fancyPrint(const std::string& opt);
	
public:
	Server();
	Server(int port, std::string password);
	~Server();

	void init();
	void run();
	
	void markPfdForPollout(int fd);
	bool disconnectClient(int fd);
	
	const std::string &getPassword() const; //+
	const std::map<int, Client*> &getClients() const; //+
	const std::map<std::string, Channel*> &getChannel() const;//+
	Client*	getClient(int fd)	const;
	Client* getClientByNickname(const std::string& nickname); //+
	Channel* getChannelByName(const std::string& name);//+

	Channel* createChannel(const std::string& channel_name, const std::string& channel_password); //+
	void deleteChannel(const std::string& channel_name);
};

struct FindByFd { //functor -> makes obj behave like a function
	int fd;

	FindByFd(int to_find) : fd(to_find) {} //constructor
	bool operator()(const pollfd& pfd) const { return pfd.fd == fd; }
};