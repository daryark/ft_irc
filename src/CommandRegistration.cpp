/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandRegistration.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:43:07 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/29 14:21:00 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Command.hpp"

void Command::executePass(Client *client)
{
    if (client->isAuthenticated())
        return client->queueMsg("462: Already registered — cannot register again.");
    if (_args.empty())
        return client->queueMsg("461: PASS Not enough parameters supplied for the command.\n");

    client->authenticate(_args[0] == _server->getPassword());
    if (!client->isAuthenticated())
        client->queueMsg("464: Password mismatch — wrong password sent with PASS.");
}

void Command::executeNick(Client *client)
{
    if(!client->isAuthenticated())
        return client->queueMsg("XXX :Register on the server first!!!!!!!!");
    if (_args.empty())    
        return client->queueMsg("431 :No nickname given");
    if (!isValidNickname())    
        return client->queueMsg("432 :Erroneous nickname");

    std::string nickname = _args[0];    
    if (nickname.empty() || nickname.find(' ') != std::string::npos) //!never goes into this if, what does it do??
        return client->queueMsg("432 " + nickname + " :Erroneous nickname");
    
    //? check working this part with const iterator    
    const std::map<int, Client *> &clients = _server->getClients();
    std::map<int, Client *>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
    {
        if (it->second->getNickname() == nickname)
            return client->queueMsg("433: Nickname is already in use.");
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
        return client->queueMsg("462: Already registered — cannot register again.");
    else if (!client->isAuthenticated())
        return client->queueMsg("XXX :Register on the server first!!!!!!!!");
    else if (client->getNickname() == "")
        return client->queueMsg("431 :No nickname given");

    // std::cout << "_args.size() = " << _args.size() << std::endl;
    if (_args.size() != 5 || _args[3] != ":")
        return client->queueMsg("461 USER :Not enough parameters");

    const std::string &username = _args[0];
    const std::string &realname = _args[3];
    client->setUserDefault(username, realname);

    if(!client->getNickname().empty() && !client->isRegistered()){
        client->setRegistered(true);
        client->queueMsg("001 " + client->getNickname() + PR_WELCOME);
    }
    // client->setRegistered(true);//?
    // client->queueMsg(PR_USAGE, B_WHITE);
}