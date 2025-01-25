/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:45:16 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/01/25 20:57:31 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Server.hpp"
//* all the '*' explanation in extra/server.about file
Server::Server(int port, std::string password): _port(port), _password(password){}

Server::~Server()
{
    close(_head_socket);
    fancy_print(PR_CLOSE);
}

void    Server::init()
{
    _head_socket = socket(AF_INET, SOCK_STREAM, 0); //*1
    if (_head_socket == -1)
        throw std::runtime_error("socket");
    
    int opt = 1; //true;
    if (setsockopt(_head_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) //*2
        throw std::runtime_error("setsockopt");
    
    if (fcntl(_head_socket,F_SETFL, O_NONBLOCK) == -1) //*3
        throw std::runtime_error("fcntl");

    std::memset(&_addr, 0, sizeof(_addr));//*4
    inet_pton(AF_INET, "0.0.0.0", &_addr.sin_addr);
    _addr.sin_port = htons(_port);//*5
    _addr.sin_family = AF_INET;
    if (bind(_head_socket, (sockaddr*)&_addr, sizeof(_addr)) == -1)
        throw std::runtime_error("bind");
    fancy_print(PR_RUN);

    if (listen(_head_socket, SOMAXCONN) == -1)
        throw std::runtime_error("listen");
    fancy_print(PR_LISTEN);
    
}

void    Server::run()
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
                disconnect_client(i); //!continue after disconnecting this client? or other clients can send some data through one poll loop at the same time???
                continue;
             }
            if (_pollfds[i].revents & POLLIN) //*6.1
            {
                if (_pollfds[i].fd == _head_socket)
                   accept_client();
                else
                    read_msg(i);
            }
            if (_pollfds[i].revents & POLLOUT)
               send_msg(i);
        }
    }
}

void    Server::accept_client()
{
    sockaddr_in client;
    socklen_t   clSize = sizeof(client);
    std::memset(&client, 0, sizeof(client));
    int client_sock = accept(_head_socket, (sockaddr*)&client, &clSize);
    if (client_sock == -1)
    {
        std::cerr << RED << "Can't connect with new client" << RE << std::endl;
        return ;
    }
    push_pollfd(client_sock, POLLIN | POLLOUT, 0);
    //!people converted ip to human readable from client.sin_addr with inet_ntoa()...add if needed later and save in constructor.
    _clients.insert(std::pair<int, Client>(client_sock, Client(client_sock, client)));
    std::cout << B_GREEN << "New client connected on socket fd: " << client_sock << RE << std::endl;
    //*send_msg() to newely connected client about the commands available to use
}

void    Server::disconnect_client(int i)
{
    (void)i;
}


void    Server::read_msg(int i)
{
    (void)i;
}

void    Server::send_msg(int i)
{
    (void)i;
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

void    Server::fancy_print(int opt)
{
    if (opt == PR_RUN)
        std::cout << B_BLUE << "Server is running on port ༼ つ " << B_RED
        << "♥" << B_BLUE << "_" << B_RED << "♥" << B_BLUE << " ༽つ "  << B_BLUE 
        << _port << ";   password: '" << _password << "'" << RE << std::endl;
    else if (opt == PR_CLOSE)
        std::cout << B_BLUE << "Server closed " << B_RED << "⊂༼" << B_BLUE
        << "´סּ" << B_RED << "︵" << B_BLUE "סּ`" << B_RED << "⊂ ༽" << RE << std::endl;
    else if (opt == PR_LISTEN)
        std::cout << B_CYAN << "Waiting for connections..." << RE << std::endl;
}