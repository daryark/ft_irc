/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandJoin.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dyarkovs <dyarkovs@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 14:41:01 by dyarkovs          #+#    #+#             */
/*   Updated: 2025/10/28 18:29:01 by dyarkovs         ###   ########.fr       */
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


//*JOIN <channel> [, <channel>...] <key> [, <key>...] |or "0"
void Command::executeJoin(Client *client)
{
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
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

    bool hasError = false;

    for (long unsigned int i = 0; i < channelNames.size(); i++) {
        hasError = false;
        Channel* channel = _server->getChannelByName(channelNames[i]);
        std::string pass = (i < channelsPasswords.size()) ? channelsPasswords[i] : "";
        if(!channel) {
            channel = _server->createChannel(channelNames[i]);
            channel->addOperator(client);
            client->joinChannel(channelNames[i]);
            channel->addClient(client);
        } else {
            if(channel->isInviteOnly() && !channel->isInvitedClient(client)) {
                _server->send_color(client->getFd(), "Channel is invite only", RED);
                hasError = true;
            }
            if(channel->hasPassword() && !channel->checkKey(channelsPasswords[i])) { //# changed checkPassword for checkKey only here!
                _server->send_color(client->getFd(), "not correct password for channel", RED);
                hasError = true;
            }
            if(channel->isFull()) {
                 _server->send_color(client->getFd(), "Channel is full", RED);
                 hasError = true;
            }
            if(channel->isMember(client)) {
                _server->send_color(client->getFd(), "Client is already joined", RED); //ignor only message, but not error without stop
                hasError = true;
            }
            if(!hasError){
                channel->addClient(client);
                client->joinChannel(channelNames[i]);
                const std::string& message = client->getNickname() + ":" + _args.back() + "\r\n";
                channel->globalMassage(client, ":" + client->getNickname() + " JOIN " + channelNames[i] + "\r\n");

            }
        }
    }
}
