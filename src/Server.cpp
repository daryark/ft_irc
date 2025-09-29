/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:45:16 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/09/29 15:26:40 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Server.hpp"
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

    std::memset(&_addr, 0, sizeof(_addr));//*4
    _addr.sin_addr.s_addr = htons(INADDR_ANY);// inet_pton(AF_INET, "0.0.0.0", &_addr.sin_addr);
    _addr.sin_port = htons(_port);//*5
    _addr.sin_family = AF_INET;
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
    //!sigint & sigstp signal catch here add
    while (true)
    {
        if (poll(_pollfds.data(), (int)_pollfds.size(), 1000) < 0) //*7
            break ;
        for (int i = 0; i < (int)_pollfds.size(); i++ )
        {
            if (_pollfds[i].revents & POLLHUP)
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
            // if (_pollfds[i].revents & POLLOUT)
            //    send_color(i, PR_WELCOME, B_GREEN);
        }
    }
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
    _clients.insert(std::pair<int, Client*>(client_sock, new Client(client_sock, client)));
    std::cout << B_GREEN << PR_CL_CONNECT << client_sock << RE << std::endl;
    send_color(client_sock, PR_WELCOME, B_GREEN);
    send_color(client_sock, PR_USAGE, I_WHITE);
}

void    Server::disconnect_client(int fd)
{
    _clients.erase(fd); //map
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
    {
        if (it->fd == fd)
        {
            std::cout << "disconnected" << std::endl;
            close(it->fd);
            _pollfds.erase(it); //vector
            break ;
        }
    }
}

void    Server::process_msg(int fd, char* buf, unsigned int len)
{
    char ss[MAX_MSG];
    strncpy(ss, buf, len);
    ss[len] = '\0';
    
    std::cout << ss << std::endl;
    send_color(fd, "msg delivered", GREEN);
}

void    Server::read_msg(int fd)
{
    char buf[MAX_MSG];
    std::cout << "fd: " << fd << std::endl;
    int recv_bytes = recv(fd, buf, MAX_MSG - 1, 0);
    std::cout << "recived bytes: " << recv_bytes << std::endl;
    usleep(100000);
    if (recv_bytes > 0)
        process_msg(fd, buf, recv_bytes);
    else
    {
        std::cerr << "Client disconnected on socket fd: " << fd <<std::endl;
        if (recv_bytes == 0)
            send_color(STDERR_FILENO,"Client disconnected on socket fd: " + fd, YELLOW);
        else if (recv_bytes == -1)
            std::cerr << "Connection problem" <<std::endl;
    disconnect_client(fd);
    }
}

void    Server::send_color(int fd, const std::string& msg, const std::string& color)
{
    std::string colored = color + msg + RE + '\n';
    int sent = send(fd, colored.c_str(), colored.length(), 0);
    if (sent < 0)
        std::cerr << "ERR send_color()" << std::endl;
    else
        std::cout << "sent bytes: " << sent << ", msg: " << msg << std::endl;
}

//------------------helpers--------------------

void    Server::push_pollfd(int fd, short event, short revent)
{
    pollfd new_pollfd;
    new_pollfd.fd = fd;
    new_pollfd.events = event;
    new_pollfd.revents = revent;
    _pollfds.push_back(new_pollfd);
}

void    Server::fancy_print(const std::string& opt)
{
    std::cout << B_BLUE << opt;
    if (opt == PR_RUN)
            std::cout << " ༼ つ " << B_RED
        << "♥" << B_BLUE << "_" << B_RED << "♥" << B_BLUE << " ༽つ "  << B_BLUE 
        << _port << ",   password '" << _password << "'";
    else if (opt == PR_CLOSE)
        std::cout << B_BLUE << opt << B_RED << " ⊂༼" << B_BLUE
        << "´סּ" << B_RED << "︵" << B_BLUE "סּ`" << B_RED << "⊂ ༽";
    std::cout << RE << std::endl;
}

const std::string& Server::getPassword() const { return _password; }
const std::map<int, Client*>& Server::getClients() const{ return _clients; }
const std::map<std::string, Channel*>& Server::getChannel() const {return _channels;}

Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Channel* Server::createChannel(const std::string& nameChannel) {
    Channel *channel = new Channel(nameChannel);
    _channels[nameChannel] = channel;
    return channel;
}