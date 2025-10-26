/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:45:16 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/26 19:51:33 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Server.hpp"
#include "../incl/CommandFactory.hpp"

volatile sig_atomic_t g_runnning = 1;//*9

void sigHandler(int)
{
    g_runnning = 0;
}

//* all the '*' explanation in extra/server.about file
Server::Server(int port, std::string password): _port(port), _password(password){}

Server::~Server()
{
    close(_head_socket);
    // _pollfds.clear();//????!
    fancy_print(PR_CLOSE);
}

void    Server::init()
{
    _head_socket = socket(AF_INET, SOCK_STREAM, 0); //*1
    if (_head_socket == -1)
        throw std::runtime_error("socket");

    int opt = 1;                                                                     // true;
    if (setsockopt(_head_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) //*2
        throw std::runtime_error("setsockopt");

    if (fcntl(_head_socket, F_SETFL, O_NONBLOCK) == -1) //*3
        throw std::runtime_error("fcntl");

    fill_sockaddr_in(_addr, AF_INET, _port, INADDR_ANY);
    if (bind(_head_socket, (sockaddr *)&_addr, sizeof(_addr)) == -1)
        throw std::runtime_error("bind");
    fancy_print(PR_RUN);

    if (listen(_head_socket, SOMAXCONN) == -1)
        throw std::runtime_error("listen");
    fancy_print(PR_LISTEN);
}

void Server::run()
{
    push_pollfd(_head_socket, POLLIN, 0); //#6
    signal(SIGINT, sigHandler);
    signal(SIGTSTP, sigHandler);
    while (g_runnning)
    {
        if (poll(_pollfds.data(), (int)_pollfds.size(), 1000) < 0) //*7
            break ;
        for (int i = 0; i < (int)_pollfds.size(); i++ )
        {
            if (_pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
             {   
                disconnect_client(i);
                continue;
             }
            if (_pollfds[i].revents & POLLIN) //*6.1
            {
                if (_pollfds[i].fd == _head_socket)
                   accept_client();
                else
                    read_msg(_pollfds[i].fd);
            }
            if (_pollfds[i].revents & POLLOUT)
               send_msg(_pollfds[i].fd);
        }
    }
}


void    Server::send_msg(int fd)
{
    Client* client = getClient(fd);
    std::deque<std::string>& msg_queue = client->getMsgQueue();
    while(!msg_queue.empty())
    {
        int n = send(fd, msg_queue.front().c_str(), msg_queue.front().size(), MSG_NOSIGNAL);
        if (n <= 0)
            break; // socket full, wait for next POLLOUT
        msg_queue.pop_front();
    }
    _pollfds[fd].events &= ~POLLOUT; // stop monitoring POLLOUT until new msg
}

void    Server::accept_client()
{
    sockaddr_in client;
    std::memset(&client, 0, sizeof(client));
    socklen_t   clSize = sizeof(client);
    int client_sock = accept(_head_socket, (sockaddr*)&client, &clSize);
    if (client_sock == -1)
    {
        std::cerr << RED << PR_CL_NOT_CONNECT << RE << std::endl;
        return ;
    }
    push_pollfd(client_sock, POLLIN | POLLOUT, 0);
    _clients.insert(std::pair<int, Client*>(client_sock, new Client(client_sock, client, this)));
    std::cout << B_GREEN << PR_CL_CONNECT << client_sock << RE << std::endl;
    send_color(client_sock, PR_IN_MSG, I_WHITE);
}

void    Server::disconnect_client(int fd)
{
    // send_color(fd, "Disconnect", RED);
    send(fd, "Disconnect", 11, MSG_NOSIGNAL);
    _clients.erase(fd); //map
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
    {
        if (it->fd == fd)
        {
            shutdown(fd, SHUT_RDWR);
            close(it->fd);
            _pollfds.erase(it); //vector
            std::cout << "disconnected" << "; ";
            break ;
        }
    }
}

void    Server::process_in_msg(int fd, char* buf, unsigned int len)
{
    char ss[MAX_MSG];
    strncpy(ss, buf, len);
    ss[len] = '\0';
    
    Command command = CommandFactory::parse(this,ss);
    command.executeCommand(getClient(fd));
    std::cout << MAGENTA << ss << RE << std::endl;//############
}

void    Server::read_msg(int fd)
{
    char buf[MAX_MSG];
    std::cout << B_YELLOW << "fd: " << fd << RE << "; ";//##########
    int recv_bytes = recv(fd, buf, MAX_MSG - 1, 0);
    std::cout << "recived bytes: " << recv_bytes << "; ";//#########
    if (recv_bytes > 0)
        process_in_msg(fd, buf, recv_bytes);
    else
    {
        if (recv_bytes == 0)
            std::cerr << "Client disconnected on socket fd: " << fd <<std::endl;
        else if (recv_bytes == -1)
            std::cerr << "Connection problem" <<std::endl;
    disconnect_client(fd);
    }
}

//*------------------helpers--------------------
void    Server::markPfdForPollout(int fd)
{
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
    {
        if (it->fd == fd)
        {
            it->events |= POLLOUT;
            break;
        }
    }
}

void    Server::send_color(int fd, const std::string& msg, const std::string& color)
{
    std::string colored = color + msg + RE + '\n';
    int sent = send(fd, colored.c_str(), colored.length(), 0);
    if (sent < 0)
        std::cerr << "ERR send_color()" << std::endl;
    // else
    //     std::cout << "sent bytes: " << sent << std::endl;//#############
}


//*getters
const std::string& Server::getPassword() const { return _password; }
const std::map<int, Client*>& Server::getClients() const{ return _clients; }
const std::map<std::string, Channel*>& Server::getChannel() const {return _channels;}

Client* Server::getClient(int fd)   const
{
    std::map<int, Client*>::const_iterator it = _clients.find(fd);
    if (it != _clients.end())
        return it->second;
    return NULL;
}

Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Channel* Server::createChannel(const std::string& nameChannel) {
    Channel *channel = new Channel(nameChannel);
    _channels[nameChannel] = channel;
    return channel;
}

Client* Server::getClientByNickname(const std::string& nickname) {
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return it->second;
        }
    }
    return NULL;
    // return std::find(_clients.begin(), _clients.end(), nickname);
}
