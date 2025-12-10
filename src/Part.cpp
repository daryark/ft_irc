#include "../incl/Command.hpp"


void Command::executePart(Client *client)
{
    if (!checkPreconditions(client, 1))
        return ;
    const std::set<std::string> channels = split<std::set<std::string> >(_args[0], ',');
    const std::string msg = (_args.size() < 3) ? "Part" : _args[2];
    leaveChannels(client, channels, msg);
}

void Command::leaveChannels(Client* client, const std::set<std::string>& channels, const std::string msg)
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
            leaveChannel(client, channel, msg);
    }
}

void Command::leaveChannel(Client* client, Channel* channel, const std::string msg)
{
    const std::string name = channel->getName();
    //need to send first + yourself
    channel->globalMessage(client,
    MSG(client->getNickname(), client->getUsername(), client->getHostname(),
    _command, name, msg), true);

    channel->removeClient(client);
    client->removeFromChannel(name);

    if (channel->getSize() == 0)
    {
        _server->deleteChannel(name);
        return ;
    }

    if (!channel->hasOperator())
    {
        Client* new_operator = *(channel->getClients().begin());
        channel->addOperator(new_operator);
        channel->globalMessage(client,
        MSG(SERVER_NAME, SERVER_NAME, SERVER_NAME, "MODE", new_operator->getNickname(),
        "is an operator now"), false);
    }

    channel->nameReplyMsg(client);
}