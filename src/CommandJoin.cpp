/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandJoin.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:41:01 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/28 22:05:26 by dyarkovs         ###   ########.fr       */
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


void Command::sendJoinInfo(Client *client, Channel *channel)
{
    // JOIN message (sent to everyone in the channel)
    std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" +
                          client->getHostname() + " JOIN :" + channel->getName() + "\r\n";
    channel->globalMessage(client, joinMsg);

    // TOPIC (332 if set, 331 if not)
    if (!channel->getTopic().empty()) {
        std::string topicMsg = ": 332 " + client->getNickname() + " " +
                               channel->getName() + " :" + channel->getTopic() + "\r\n";
        client->queueMsg(topicMsg);
    } else {
        std::string noTopicMsg = ": 331 " + client->getNickname() + " " +
                                 channel->getName() + " :No topic is set\r\n";
        client->queueMsg(noTopicMsg);
    }

    // NAMES list (353)
    std::string namesList;
    const std::set<Client *> &clients = channel->getClients();

    for (std::set<Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        Client *member = *it;
        if (channel->isOperator(member))
            namesList += "@" + member->getNickname() + " ";
        else
            namesList += member->getNickname() + " ";
    }

    std::string namesMsg = ": 353 " + client->getNickname() + " = " +
                           channel->getName() + " :" + namesList + "\r\n";
    client->queueMsg(namesMsg);

    // End of NAMES list (366)
    std::string endMsg = ": 366 " + client->getNickname() + " " +
                         channel->getName() + " :End of /NAMES list.\r\n";
    client->queueMsg(endMsg);
}


//*JOIN <channel> [, <channel>...] <key> [, <key>...] |or "0"
void Command::executeJoin(Client *client)
{
    // if(!client->isRegistered())
    //     return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    if(_args.empty())
        return _server->send_color(client->getFd(), "461 JOIN :Not enough parameters", RED);
    if (_args.size() == 1 && _args[0] == "0")
        std::cout << BG_RED << "leave all the channels request from user: " << client->getNickname() << RE << std::endl;

    const std::vector<std::string> channelNames = split(_args[0], ',');
    //print channel names
    for(size_t i = 0; i < channelNames.size(); i++) {
        if(channelNames[i].empty() || (channelNames[i][0] != '#' && channelNames[i][0] != '&')) {
            std::cout << "Invalid channel name: " << channelNames[i] << std::endl;
        }
        std::cout << "Channel to join: " << channelNames[i] << std::endl;
    }
    std::vector<std::string> channelsPasswords;
    if(_args.size() == 2)
        channelsPasswords = split(_args[1], ',');


    for (long unsigned int i = 0; i < channelNames.size(); i++) {
        Channel* channel = _server->getChannelByName(channelNames[i]);
        std::string pass = (i < channelsPasswords.size()) ? channelsPasswords[i] : "";
        if(!channel) {
            channel = _server->createChannel(channelNames[i]);
            channel->addOperator(client);
            channel->addClient(client);
            client->joinChannel(channelNames[i]);
            sendJoinInfo(client, channel);
        } else {
            if(channel->isInviteOnly() && !channel->isInvitedClient(client)) {
                _server->send_color(client->getFd(), "Channel is invite only", RED);
                continue;
            }
            if(channel->hasPassword() && !channel->checkKey(channelsPasswords[i])) { //# changed checkPassword for checkKey only here!
                _server->send_color(client->getFd(), "not correct password for channel", RED);
                continue;
            }
            if(channel->isFull()) {
                 _server->send_color(client->getFd(), "Channel is full", RED);
                 continue;
            }
            if(channel->isMember(client)) {
                _server->send_color(client->getFd(), "Client is already joined", RED); //ignor only message, but not error without stop
                continue;
            }
            channel->addClient(client);
            client->joinChannel(channelNames[i]);
            // const std::string& message = client->getNickname() + ":" + _args.back() + "\r\n";
            sendJoinInfo(client, channel);

            //channel->globalMessage(client, ":" + client->getNickname() + " JOIN " + channelNames[i] + "\r\n");      
        }
    }
}
