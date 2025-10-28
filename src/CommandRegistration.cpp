/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandRegistration.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:43:07 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/28 14:44:47 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Command.hpp"

void Command::executePass(Client *client)
{
    if (client->isAuthenticated())
        return _server->send_color(client->getFd(), "462: Already registered — cannot register again.", RED);
    if (_args.empty())
        return _server->send_color(client->getFd(), "461: PASS Not enough parameters supplied for the command.\n", RED);

    client->authenticate(_args[0] == _server->getPassword());
    if (!client->isAuthenticated())
        _server->send_color(client->getFd(), "464: Password mismatch — wrong password sent with PASS.", RED);
}

void Command::executeNick(Client *client)
{
    if(!client->isAuthenticated())
        return _server->send_color(client->getFd(), "XXX :Register on the server first!!!!!!!!", RED);
    if (_args.empty())    
        return _server->send_color(client->getFd(), "431 :No nickname given", RED);
    if (!isValidNickname())    
        return _server->send_color(client->getFd(), "432 :Erroneous nickname", RED);

    std::string nickname = _args[0];    
    if (nickname.empty() || nickname.find(' ') != std::string::npos) //!never goes into this if, what does it do??
        return _server->send_color(client->getFd(), "432 " + nickname + " :Erroneous nickname", RED);
    
    //? check working this part with const iterator    
    const std::map<int, Client *> &clients = _server->getClients();
    std::map<int, Client *>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
    {
        if (it->second->getNickname() == nickname)
            return _server->send_color(client->getFd(), "433: Nickname is already in use.", RED);
    }        
    client->setNickname(_args[0]);
}    

bool Command::isValidNickname() const
{
    // reject if args array has more than 1 element
    if (_args.size() != 1)
        return false;

    const std::string &nick = _args[0];    
    if (nick.empty() || nick.size() > 9)
        return false;

    // allowed special chars (RFC 2812)    
    const std::string special = "[]\\`_^{|}";

    // first char: letter or special
    char c = nick[0];
    if (!isalpha(c) && special.find(c) == std::string::npos)
        return false;

    // remaining chars: letter / digit / special / '-'    
    for (size_t i = 1; i < nick.size(); ++i)
    {
        c = nick[i];
        if (!isalnum(c) && special.find(c) == std::string::npos && c != '-')
            return false;
    }        
    return true;
}    

void Command::executeUser(Client *client)
{
    if (client->isRegistered())
        return _server->send_color(client->getFd(), "462: Already registered — cannot register again.", RED);
    else if (!client->isAuthenticated())
        return _server->send_color(client->getFd(), "XXX :Register on the server first!!!!!!!!", RED);
    else if (client->getNickname() == "")
        return _server->send_color(client->getFd(), "431 :No nickname given", RED);

    // std::cout << "_args.size() = " << _args.size() << std::endl;
    if (_args.size() != 5 || _args[3] != ":")
        return _server->send_color(client->getFd(), "461 USER :Not enough parameters", RED);

    const std::string &username = _args[0];
    const std::string &realname = _args[3];
    client->setUserDefault(username, realname);

    if(!client->getNickname().empty() && !client->isRegistered()){
        client->setRegistered(true);
        _server->send_color(client->getFd(), "001 " + client->getNickname() + PR_WELCOME, B_GREEN);
    }
    // client->setRegistered(true);//?
    // _server->send_color(client->getFd(), PR_USAGE, B_WHITE);
}