#include "../incl/Command.hpp"

void Command::executeKick(Client *client)
{
    if (!checkPreconditions(client, 4))
        return ;

    //_args[0]- it depends of place where sen the command 
    // _args[0] = my_irc - name of the server
    // _args[0] = #target_channel_name 
    const std::vector<std::string> channels = splitVec(_args[1], ',');
    const std::vector<std::string> nicks = splitVec(_args[3], ',');
    const std::string comment = _args.size() > 3 ? joinVecIntoStr(_args.begin() + 3, _args.end()) : "is kicked";
    

    // std::cout << "chanel = " << channels[0] << std::endl;
    // std::cout << "nicks = " << nicks[0] << std::endl;
    // std::cout << "comment = " << comment;

    if (channels.size() == 1)
    {
        if (!canKickFromChannel(client, channels[0]))
            return ;
        for (size_t i = 0; i < nicks.size(); i++)
            executeKickOne(client, channels[0], nicks[i], comment);
    }
    else
    {
        if (channels.size() != nicks.size())
            return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), _command));
        for (size_t i = 0; i < channels.size(); i++)
        {
            if (canKickFromChannel(client, channels[i]))
                executeKickOne(client, channels[i], nicks[i], comment);
        }
    }
}

void Command::executeKickOne(Client* client, const std::string& channel_name, const std::string& target_nick, const std::string& comment)
{
    Channel* channel = _server->getChannelByName(channel_name);
    Client* target_client = _server->getClientByNickname(target_nick);
    if(!target_client)
        return client->queueMsg(ERR_NOSUCHNICK(target_nick));
    else if(!channel->isMember(target_client))
        return client->queueMsg(ERR_USERNOTINCHANNEL(target_nick, channel_name));
    
    channel->globalMessage(client, MSG(client->getNickname(), client->getUsername(), client->getHostname(), _command, channel_name, comment), true);

    channel->removeClient(target_client);
    target_client->removeFromChannel(channel_name);

    if (channel->getSize() == 0){
        // _server->deleteChannel(target_nick); //????
        _server->deleteChannel(channel_name);
        return ;
    }

    channel->globalMessage(client, RPL_NAMREPLY(client->getNickname(), channel->getName(), formChannelMembersList(channel)), true);
	channel->globalMessage(client, RPL_ENDOFNAMES(client->getNickname(), channel->getName()), true);
}

bool Command::canKickFromChannel(Client* client, const std::string& channel_name)
{
    Channel* channel = _server->getChannelByName(channel_name);
    if(!channel)
    {
        client->queueMsg(ERR_NOSUCHCHANNEL(channel_name));
        return false;
    }
    else if(!channel->isOperator(client))
    {
        client->queueMsg(ERR_CHANOPRIVSNEEDED(channel_name));
        return false;
    }
    return true;
}
