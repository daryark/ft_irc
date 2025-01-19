/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:45:16 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/01/19 23:11:06 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Server.hpp"
//* all the '*' explanation in extra/server.about file
Server::Server(int port, std::string password): _port(port), _password(password)
{

}

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
    push_pollfd({_head_socket, POLLIN, 0}); //#6
    // _pollfds.push_back(serv_pollfd);
    //!sigint & sigstp signal catch here add
    while (true)
    {
        if (poll(_pollfds.data(), _pollfds.size(), 1000) < 0) //*7
            break ;
        for (int i = 0; i < _pollfds.size(); i++ )
        {
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
        //!find out, where and how i init map for all clinets overall, watch The Cherno map till end
        //! maybe change addr ip of client from intet to int/ascii ?
        //!new Client*
        //!push to client map (insert or/and make_pair(what is last one))
    }
}

void    Server::read_msg(int i)
{
}

void    Server::send_msg(int i)
{
    
}

void    Server::accept_client()
{
    sockaddr_in client;
    std::memset(&client, 0, sizeof(client));
    int client_sock = accept(_head_socket, (sockaddr*)&client, sizeof(client));
    if (client_sock == -1)
    {
        std::cerr << RED << "Can't connect with new client" << RE << std::endl;
        return ;
    }
    push_pollfd({client_sock, POLLIN | POLLOUT, 0});
    
}

//------------------helpers--------------------

void    Server::push_pollfd(pollfd pfd)
{
    _pollfds.push_back(pfd);
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