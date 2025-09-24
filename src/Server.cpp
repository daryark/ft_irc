/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:45:16 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/09/24 13:26:13 by dyarkovs         ###   ########.fr       */
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
    _addr.sin_addr.s_addr = htons(INADDR_ANY);// inet_pton(AF_INET, "0.0.0.0", &_addr.sin_addr);
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
                disconnect_client(i);
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
    std::memset(&client, 0, sizeof(client));
    socklen_t   clSize = sizeof(client);
    int client_sock = accept(_head_socket, (sockaddr*)&client, &clSize);
    if (client_sock == -1)
    {
        std::cerr << RED << PR_CL_NOT_CONNECT << RE << std::endl;
        return ;
    }
    push_pollfd(client_sock, POLLIN | POLLOUT, 0);
    _clients.insert(std::pair<int, Client>(client_sock, Client(client_sock, client)));
    //!people converted ip to human readable from client.sin_addr with inet_ntoa()...add if needed later and save in constructor
    char *ip = inet_ntoa(client.sin_addr);
    std::cout << "sin_addr: " << client.sin_addr.s_addr << " ip: " << ip << std::endl; //? check, if we save the whole client sockaddr_in or only ip of the client in the pairs struct
    std::cout << B_GREEN << PR_CL_CONNECT << client_sock << RE << std::endl;
    //*send_msg() to newely connected client about the commands available to use
    send_msg(client_sock);
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
    std::string data = "Welcome on the server";
    std::cout << B_GREEN;
    send(i, data.c_str(), data.length(), 0);
    std::cout << RE;
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