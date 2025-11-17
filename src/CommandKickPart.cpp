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
    //*PART commented. check if it fits here ideally -> separate into fn. else, modify -> how much???
    //!most probably different check for +o condition (KICKer must be an op. - PART person - must not, but can)
    
    if (channels.size() == 1)
    {
        Channel* channel = _server->getChannelByName(channels[0]);
        if(!channel)
            return client->queueMsg(ERR_NOSUCHCHANNEL(channels[0]));
        if(!channel->isOperator(client))
            return client->queueMsg(ERR_CHANOPRIVSNEEDED(channels[0])); //!op check
        for (size_t i = 0; i < nicks.size(); i++)
        {
                Client* targetClient = _server->getClientByNickname(nicks[i]);
                if(!targetClient)
                    client->queueMsg(ERR_NOSUCHNICK(nicks[i]));
                else if(!channel->isMember(targetClient))
                    client->queueMsg(ERR_USERNOTINCHANNEL(nicks[i], channels[0]));
                else
                {
                    channel->removeClient(targetClient);
                    targetClient->removeFromChannel(channels[0]);
                    channel->globalMessage(client,
                    MSG(client->getNickname(), client->getUsername(), client->getHostname(),
                    "KICK", channels[0], "is kicked"));   
                }
        }
    }
    else
    {
        if (channels.size() != nicks.size())
            return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"));
        for (size_t i = 0; i < channels.size(); i++)
        {
            Channel* channel = _server->getChannelByName(channels[0]);
            if(!channel)
                client->queueMsg(ERR_NOSUCHCHANNEL(channels[0]));
            else if(!channel->isOperator(client))
                client->queueMsg(ERR_CHANOPRIVSNEEDED(channels[0])); //!op check
            else
            {
                Client* targetClient = _server->getClientByNickname(nicks[i]);
                if(!targetClient)
                    client->queueMsg(ERR_NOSUCHNICK(nicks[i]));
                else if(!channel->isMember(targetClient))
                    return client->queueMsg(ERR_USERNOTINCHANNEL(nicks[i], channels[0]));
                else
                {
                    channel->removeClient(targetClient);
                    targetClient->removeFromChannel(channels[0]);
                    if (channel->getSize() == 0)
                        _server->deleteChannel(nicks[i]);
                    else
                    {
                        channel->globalMessage(client,
                        MSG(client->getNickname(), client->getUsername(), client->getHostname(),
                        "KICK", channels[0], "is kicked"));
                        if (!channel->hasOperator())
                        {
                            std::cout << BG_GREEN << "has operator: " << channel->hasOperator() << RE << std::endl;
                            channel->addOperator(*(channel->getMembersBegin()));
                            std::cout << BG_GREEN << "is operator: " << channel->isOperator(*(channel->getMembersBegin())) << RE << std::endl;
                        //?!tell in the channel that Client is now an operator!?
                        //*solution -> send MODE +o command and it will auto send globalMessage to the channel with needed syntax
                        }
                    }
                }
            }
        }
    }
}

void Command::executePart(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getNickname(), "PART"));
    if(_args.size() < 1)
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "PART"));
    const std::set<std::string> nicks = splitSet(_args[0], ',');
    const std::string part_msg = (_args.size() > 1 
        ? joinVecIntoStr(_args.begin() + 1, _args.end()) : "Leaving\r\n");  //#\r\n ???
    
    for (size_t i = 0; i < nicks.size(); i++)
    {
        Channel *channel = _server->getChannelByName(nicks[i]);
        if (!channel)
            client->queueMsg(ERR_NOSUCHCHANNEL(nicks[i]));
        else if (!channel->isMember(client))
            client->queueMsg(ERR_NOTONCHANNEL(nicks[i]));
        else
        {
            client->removeFromChannel(nicks[i]);
            if (channel->isOperator(client))
                channel->removeOperator(client);
            channel->removeClient(client);
            targetClient->removeFromChannel(channels[0]);
            if (channel->getSize() == 0)
                _server->deleteChannel(nicks[i]);
            else
            {
                channel->globalMessage(client,
                MSG(client->getNickname(), client->getUsername(), client->getHostname(),
                "PART", nicks[i], part_msg));
                if (!channel->hasOperator())
                {
                    std::cout << BG_GREEN << "has operator: " << channel->hasOperator() << RE << std::endl;
                    channel->addOperator(*(channel->getMembersBegin()));
                    std::cout << BG_GREEN << "is operator: " << channel->isOperator(*(channel->getMembersBegin())) << RE << std::endl;
                //?!tell in the channel that Client is now an operator!?
                //*solution -> send MODE +o command and it will auto send globalMessage to the channel with needed syntax
                }
            }
        }
        
    }
    std::cout << "PART channels: " << _args[0];
}