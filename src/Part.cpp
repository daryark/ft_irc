#include "../incl/Command.hpp"


void Command::executePart(Client *client)
{
    if (!checkPreconditions(client, 1))
        return ;
    const std::set<std::string> channels = splitSet(_args[0], ',');
    leaveChannels(client, channels);
}

void Command::leaveChannels(Client* client, const std::set<std::string>& channels)
{
    const std::string part_msg = (_args.size() > 1 ? joinVecIntoStr(_args.begin() + 1, _args.end()) : "Leaving");
    
    for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        Channel *channel = _server->getChannelByName(*it);
        if (!channel)
            client->queueMsg(ERR_NOSUCHCHANNEL(*it));
        else if (!channel->isMember(client))
            client->queueMsg(ERR_NOTONCHANNEL(*it));
        else
        {
            if (channel->getSize() == 1)
                return _server->deleteChannel(*it);
            if (channel->isOperator(client))
                channel->removeOperator(client);
            channel->removeClient(client);
            client->removeFromChannel(*it);
            
            channel->globalMessage(client,
            MSG(client->getNickname(), client->getUsername(), client->getHostname(), _command, *it, part_msg));
            
            if (!channel->hasOperator())
            {
                channel->addOperator(*(channel->getClients().begin()));
                channel->globalMessage(client,
                MSG(SERVER_NAME, SERVER_NAME, SERVER_NAME, "MODE", *it, "is an operator now"));
            }
        }
    }
}