/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPrivmsg.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:40:12 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/31 21:19:49 by dyarkovs         ###   ########.fr       */
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
    if(_args.size() < 2)
        return client->queueMsg(ERR_NEEDMOREPARAMS(nick, "PRIVMSG"));
    if(_args[0] == ":")
         return client->queueMsg(ERR_NORECIPIENT(nick, "PRIVMSG"));
    if (_args[_args.size() - 2] != ":")
        return client->queueMsg(ERR_NOTEXTTOSEND(nick));
 
    for (unsigned int i = 0; i < (_args.size() - 2); i++)
    {
        const std::string &target = _args[i];

        if(target[0] == '#' || target[0] == '&')
            executePrivmsgToChannel(client, target);
        else
            executePrivmsgToClient(client, target);
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
        return client->queueMsg(ERR_NOSUCHNICK(target));
    if(!channel->isMember(client))
        return client->queueMsg(ERR_CANNOTSENDTOCHAN(target));
    channel->globalMessage(client,
        MSG(client->getNickname(), client->getUsername(), client->getHostname(), "PRIVMSG", target, _args.back()));

}