/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPrivmsg.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:40:12 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/11/06 20:13:30 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Command.hpp"

//    Command: PRIVMSG
//    Parameters: <msgtarget>{,<msgtarget>} :<text to be sent>
void Command::executePrivmsg(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getSafeNickname(), "PRIVMSG"));

    const std::string& nick = client->getNickname();
    if(_args.size() == 1)
        return client->queueMsg(ERR_NEEDMOREPARAMS(nick, "PRIVMSG"));
    if (_args[1] == ":" && _args.size() == 2)
        return client->queueMsg(ERR_NOTEXTTOSEND(nick));
    if(_args[0] == ":" || _args.size() != 3)
        return client->queueMsg(ERR_NORECIPIENT(nick, "PRIVMSG"));
 
    const std::set<std::string> targets = splitSet(_args[0], ',');

    for (std::set<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
    {
        if((*it)[0] == '#' || (*it)[0] == '&')
            executePrivmsgToChannel(client, *it);
        else
            executePrivmsgToClient(client, *it);
    }
}

void Command::executePrivmsgToClient(Client* client, const std::string& target)
{
    if(target == client->getNickname())
            return ;
    Client* targetClient = _server->getClientByNickname(target);
    if(!targetClient)
        return client->queueMsg(ERR_NOSUCHNICK(target));
    targetClient->queueMsg(
        MSG(client->getNickname(), client->getUsername(), client->getHostname(), "PRIVMSG", target, _args.back()));

}

void Command::executePrivmsgToChannel(Client* client, const std::string& target)
{
    Channel* channel = _server->getChannelByName(target);
    if(!channel)
        return client->queueMsg(ERR_NOSUCHCHANNEL(target));
    if(!channel->isMember(client))
        return client->queueMsg(ERR_CANNOTSENDTOCHAN(target));
    channel->globalMessage(client,
        MSG(client->getNickname(), client->getUsername(), client->getHostname(), "PRIVMSG", target, _args.back()));

}