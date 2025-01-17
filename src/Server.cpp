/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:45:16 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/01/17 18:36:16 by dyarkovs         ###   ########.fr       */
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
    
    if (fcntl(_head_socket,F_SETFL, O_NONBLOCK) == -1) //*5
        throw std::runtime_error("fcntl");
    
    std::memset(&_addr, 0, sizeof(_addr));//*3
    inet_pton(AF_INET, "0.0.0.0", &_addr.sin_addr);
    _addr.sin_port = htons(_port);//*4
    _addr.sin_family = AF_INET;
    if (bind(_head_socket, (sockaddr*)&_addr, sizeof(_addr)) == -1)
        throw std::runtime_error("bind");
    fancy_print(PR_RUN);
    if (listen(_head_socket, SOMAXCONN) == -1)
        throw std::runtime_error("listen");
    fancy_print(PR_LISTEN);
    
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