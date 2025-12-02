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
    std::set<std::string> copy = channels;
    for (std::set<std::string>::iterator it = copy.begin(); it != copy.end(); it++)
    {
        Channel* channel = _server->getChannelByName(*it);
        if (!channel)
            client->queueMsg(ERR_NOSUCHCHANNEL(*it));
        else if (!channel->isMember(client))
            client->queueMsg(ERR_NOTONCHANNEL(*it));
        else
            leaveChannel(client, channel);
    }
}

void Command::leaveChannel(Client* client, Channel* channel)
{
    const std::string part_msg = (!_args.empty() ? joinVecIntoStr(_args.begin() + 1, _args.end()) : "Leaving");
    const std::string name = channel->getName();
    if (channel->getSize() == 1)
    {
        _server->deleteChannel(name);
        client->removeFromChannel(name);
        return ;
    }
    if (channel->isOperator(client))
        channel->removeOperator(client);
    channel->removeClient(client);
    client->removeFromChannel(channel->getName());
    channel->globalMessage(client,
    MSG(client->getNickname(), client->getUsername(), client->getHostname(),
    _command, name, part_msg), false);

    if (!channel->hasOperator())
    {
        Client* new_operator = *(channel->getClients().begin());
        channel->addOperator(new_operator);
        channel->globalMessage(client,
        MSG(SERVER_NAME, SERVER_NAME, SERVER_NAME, "MODE", new_operator->getNickname(),
        "is an operator now"), false);
    }
}