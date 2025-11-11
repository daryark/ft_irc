#include "../incl/Command.hpp"

//#helper function, move out of here
static inline const std::string joinVecIntoStr(std::vector<std::string>::const_iterator start,
                                               std::vector<std::string>::const_iterator end)
{
    std::string result;
    for (std::vector<std::string>::const_iterator it = start; it != end; it++)
        result += (*it + " ");
    if (!result.empty())
        return result.substr(0, result.length() - 2);
    return "";
}

//!MUST be 1-to-1 channel and user amount of elems
//!or 1 channel -> 1-to multiple clients.
//#no valid kicking yourself
//#join 0 - add working like PART loop
void Command::executeKick(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getNickname(), "KICK"));
    if(_args.size() != 2)
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"));
    
    const std::vector<std::string> channels = splitVec(_args[0], ',');
    const std::vector<std::string> nicks = splitVec(_args[1], ',');
    if (channels.size() > 1 && channels.size() != nicks.size())
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"));
    // const std::string &channel_name = _args[0];
    // const std::string &target_nick = _args[1];
    //!loop the channels if multiple
    //!separate if, if 1 channel, or in loop condition?
    //*PART commented. check if it fits here ideally -> separate into fn. else, modify -> how much???
    //!most probably different check for +o condition (KICKer must be an op. - PART person - must not, but can)
    // Channel *channel = _server->getChannelByName(*it);
    //     if (!channel)
    //         client->queueMsg(ERR_NOSUCHCHANNEL(*it));
    //     else if (!channel->isMember(client))
    //         client->queueMsg(ERR_NOTONCHANNEL(*it));
    //     else
    //     {
    //         client->removeFromChannel(*it);
    //         if (channel->isOperator(client))
    //             channel->removeOperator(client);
    //         channel->removeClient(client);
    //         if (channel->getSize() == 0)
    //             _server->deleteChannel(*it);
    //         else
    //         {
    //             if (!channel->hasOperator())
    //             {
    //                 std::cout << BG_GREEN << "has operator: " << channel->hasOperator() << RE << std::endl;
    //                 channel->addOperator(*(channel->getMembersBegin()));
    //                 std::cout << BG_GREEN << "is operator: " << channel->isOperator(*(channel->getMembersBegin())) << RE << std::endl;
    //             //?!tell in the channel that Client is now an operator!?
    //             //*solution -> send MODE +o command and it will auto send globalMessage to the channel with needed syntax
    //             }
    //             channel->globalMessage(client,
    //             MSG(client->getNickname(), client->getUsername(), client->getHostname(),
    //             "PART", *it, part_msg));
    //         }
    //     }
    std::string comment = (_args.size() >= 3) ? _args[2] : "No reason\r\n"; //#\r\n ???

    Channel* channel = _server->getChannelByName(channel_name);
    if(!channel)
        return client->queueMsg(ERR_NOSUCHCHANNEL(channel_name));
    if(!channel->isOperator(client))
        return client->queueMsg(ERR_CHANOPRIVSNEEDED(channel_name));
    Client* targetClient = _server->getClientByNickname(target_nick);
    if(!targetClient)
        return client->queueMsg(ERR_NOSUCHNICK(target_nick));
    if(!channel->isMember(targetClient))
        return client->queueMsg(ERR_USERNOTINCHANNEL(target_nick, channel_name));
    channel->removeClient(targetClient);
    targetClient->removeFromChannel(channel_name);
    channel->globalMessage(client,
    MSG(client->getNickname(), client->getUsername(), client->getHostname(),
    "KICK", channel_name, comment));
}

void Command::executePart(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getNickname(), "PART"));
    if(_args.size() < 1)
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "PART"));
    const std::set<std::string> targets = splitSet(_args[0], ',');
    const std::string part_msg = (_args.size() > 1 
        ? joinVecIntoStr(_args.begin() + 1, _args.end()) : "Leaving\r\n");  //#\r\n ???
    
    for (std::set<std::string>::const_iterator it = targets.begin(); it != targets.end(); it++)
    {
        Channel *channel = _server->getChannelByName(*it);
        if (!channel)
            client->queueMsg(ERR_NOSUCHCHANNEL(*it));
        else if (!channel->isMember(client))
            client->queueMsg(ERR_NOTONCHANNEL(*it));
        else
        {
            client->removeFromChannel(*it);
            if (channel->isOperator(client))
                channel->removeOperator(client);
            channel->removeClient(client);
            if (channel->getSize() == 0)
                _server->deleteChannel(*it);
            else
            {
                if (!channel->hasOperator())
                {
                    std::cout << BG_GREEN << "has operator: " << channel->hasOperator() << RE << std::endl;
                    channel->addOperator(*(channel->getMembersBegin()));
                    std::cout << BG_GREEN << "is operator: " << channel->isOperator(*(channel->getMembersBegin())) << RE << std::endl;
                //?!tell in the channel that Client is now an operator!?
                //*solution -> send MODE +o command and it will auto send globalMessage to the channel with needed syntax
                }
                channel->globalMessage(client,
                MSG(client->getNickname(), client->getUsername(), client->getHostname(),
                "PART", *it, part_msg));
            }
        }
        
    }
    std::cout << "PART channels: " << _args[0];
}