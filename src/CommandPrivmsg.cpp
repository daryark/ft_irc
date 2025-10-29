/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPrivmsg.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:40:12 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/29 21:05:02 by dyarkovs         ###   ########.fr       */
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
         return client->queueMsg("411 :No recipient given (PRIVMSG)");
    if (_args[_args.size() - 2] != ":")
        return client->queueMsg("412 :No text to send");
    const std::string& message = ":" + client->getNickname() + " PRIVMSG ";
    
    
    for (unsigned int i = 0; i < (_args.size() - 2); i++)
    {
        const std::string &target = _args[i];
        if(target == client->getNickname())
            continue;
        if(target[0] == '#' || target[0] == '&') {
            Channel* channel = _server->getChannelByName(target);
            if(!channel)
                return client->queueMsg("403 " + target + " :No such channel");
            if(!channel->isMember(client))
                return client->queueMsg("404 " + target + " :Cannot send to channel");
    
            channel->globalMessage(client, message + target + ":" + _args.back() + "\r\n");
        } else {
            Client* targetClient = _server->getClientByNickname(target);
            if(!targetClient)
                return client->queueMsg("401 " + target + " :No such nick");
            targetClient->queueMsg(message + target + ":" + _args.back() + "\r\n");
        }
    }
}