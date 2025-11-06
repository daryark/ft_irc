/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandJoin.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:41:01 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/11/06 15:49:23 by dyarkovs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/Command.hpp"


//#out of class, move to the helper file?!
std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

const std::string Command::formChannelMembersList(Channel *channel)
{
    std::string names_list;
    const std::set<Client *> &clients = channel->getClients();

    for (std::set<Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        Client *member = *it;
        if (channel->isOperator(member))
            names_list += "@";
        names_list += member->getNickname() + " ";
    }
    return names_list;
}

void Command::sendJoinInfo(Client *client, Channel *channel)
{
    // JOIN message (sent to everyone in the channel)
    std::string join_msg = MSG_PREFIX(client->getNickname(), client->getUsername(),
        client->getHostname(), "JOIN") + ":" + channel->getName() + "\r\n";
    channel->globalMessage(client, join_msg);

    if (!channel->getTopic().empty())
        client->queueMsg(RPL_TOPIC(channel->getName(), channel->getTopic()));
    else
        client->queueMsg(RPL_NOTOPIC(channel->getName()));

    client->queueMsg(RPL_NAMREPLY(client->getNickname(), channel->getName(), formChannelMembersList(channel)));
    client->queueMsg(RPL_ENDOFNAMES(client->getNickname(), channel->getName()));
}


//*JOIN <channel> [, <channel>...] <key> [, <key>...] |or "0"
void Command::executeJoin(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getNickname(), "JOIN"));
    if(_args.empty())
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN"));
    if (_args.size() == 1 && _args[0] == "0")
        std::cout << BG_RED << "leave all the channels request from user: " << client->getNickname() << RE << std::endl;

    const std::vector<std::string> channel_names = split(_args[0], ',');
    //print channel names
    for(size_t i = 0; i < channel_names.size(); i++) {
        if(channel_names[i].empty() || (channel_names[i][0] != '#' && channel_names[i][0] != '&'))
            std::cout << "Invalid channel name: " << channel_names[i] << std::endl;
        std::cout << "Channel to join: " << channel_names[i] << std::endl;
    }
    std::vector<std::string> channels_passwords;
    if(_args.size() == 2)
        channels_passwords = split(_args[1], ',');


    for (long unsigned int i = 0; i < channel_names.size(); i++) {
        Channel* channel = _server->getChannelByName(channel_names[i]);
        std::string pass = (i < channels_passwords.size()) ? channels_passwords[i] : "";
        if(!channel) {
            channel = _server->createChannel(channel_names[i]);
            channel->addOperator(client);
            channel->addClient(client);
            client->joinChannel(channel_names[i]);
            sendJoinInfo(client, channel);
        } else {
            if(channel->isInviteOnly() && !channel->isInvitedClient(client)) {
                client->queueMsg("Channel is invite only");
                continue;
            }
            if(channel->hasPassword() && !channel->checkKey(channels_passwords[i])) { //# changed checkPassword for checkKey only here!
                client->queueMsg("not correct password for channel");
                continue;
            }
            if(channel->isFull()) {
                 client->queueMsg("Channel is full");
                 continue;
            }
            if(channel->isMember(client)) {
                client->queueMsg("Client is already joined"); //ignor only message, but not error without stop
                continue;
            }
            channel->addClient(client);
            client->joinChannel(channel_names[i]);
            // const std::string& message = client->getNickname() + ":" + _args.back() + "\r\n";
            sendJoinInfo(client, channel);

            //channel->globalMessage(client, ":" + client->getNickname() + " JOIN " + channel_names[i] + "\r\n");      
        }
    }
}
