/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_helpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 13:18:06 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/29 14:07:08 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Server.hpp"

void    Server::pushPollfd(int fd, short event, short revent)
{
    pollfd new_pollfd;
    new_pollfd.fd = fd;
    new_pollfd.events = event;
    new_pollfd.revents = revent;
    _pollfds.push_back(new_pollfd);
}

void    Server::fillSockaddrIn(struct sockaddr_in& addr, short int in_family, unsigned short int in_port, uint32_t s_addr)
{
    std::memset(&addr, 0, sizeof(addr));//*4
    addr.sin_family = in_family;
    addr.sin_port = htons(in_port);//*5
    addr.sin_addr.s_addr = htonl(s_addr);// inet_pton(AF_INET, "0.0.0.0", &_addr.sin_addr);
}

void    Server::fancyPrint(const std::string& opt)
{
    std::cout << std::endl << B_BLUE << opt;
    if (opt == PR_RUN)
            std::cout << " ༼ つ " << B_RED
        << "♥" << B_BLUE << "_" << B_RED << "♥" << B_BLUE << " ༽つ "  << B_BLUE 
        << _port << ",   password '" << _password << "'";
    else if (opt == PR_CLOSE)
        std::cout << B_RED << " ⊂༼" << B_BLUE
        << "´סּ" << B_RED << "︵" << B_BLUE "סּ`" << B_RED << "⊂ ༽";
    std::cout << RE << std::endl;
}