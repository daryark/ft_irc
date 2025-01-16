/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 20:45:16 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/01/16 19:25:35 by dyarkovs         ###   ########.fr       */
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
}
Server::init()
{
    _head_socket = socket(AF_INET, SOCK_STREAM, 0); //*1
    if (_head_socket == -1)
        throw std::runtime_error("socket");
      
    int opt = 1; //true;
    if (setsockopt(_head_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) //*2
        throw std::runtime_error("setsockopt");
    
    std::memset(&_addr, 0, sizeof(&_addr));//*3
    _addr.sin_family = AF_INET;
    _addr.sin_addr = htons(_port);//*4
    inet_pton(AF_INET, "0.0.0.0", &_addr.sin_addr);
    if (bind(servSockListen, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
        throw std::runtime_error("bind");
}