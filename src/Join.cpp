#include "../incl/Command.hpp"

//#move smwhere to helpers!
inline bool isValidChannelName(const std::string &channel_name)
{
    if (channel_name[0] != '#' && channel_name[0] != '&')
        return false;
    for (size_t i = 1; i < channel_name.size(); i++)
    {
        if (!isalnum(channel_name[i]))
            return false;
    }
    return true;
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
    channel->globalMessage(client, join_msg, true);

    if (!channel->getTopic().empty())
        client->queueMsg(RPL_TOPIC(channel->getName(), channel->getTopic()));
    else
        client->queueMsg(RPL_NOTOPIC(channel->getName()));

    client->queueMsg(RPL_NAMREPLY(client->getNickname(), channel->getName(), formChannelMembersList(channel)));
    client->queueMsg(RPL_ENDOFNAMES(client->getNickname(), channel->getName()));
}

//JOIN <channel> [, <channel>...] <key> [, <key>...] |or "0"
void Command::executeJoin(Client *client)
{
    if (!checkPreconditions(client, 1))
        return;
    if (_args.size() == 1 && _args[0] == "0")
        return leaveChannels(client, client->getJoinedChannels());

    const std::vector<std::string> channel_names = splitVec(_args[0], ',');
    std::vector<std::string> channels_passwords;
    if (_args.size() == 2)
        channels_passwords = splitVec(_args[1], ',');
    size_t p = 0;
    for (size_t i = 0; i < channel_names.size(); i++)
    {
        Channel *channel = _server->getChannelByName(channel_names[i]);

        std::string pass = (!channel || (channel && channel->hasPassword())) && p < channels_passwords.size()
                               ? channels_passwords[p++]
                               : "";
        std::cout << BG_GREEN << "channel: " << channel_names[i] << ", password: '" << pass << "'" << RE << std::endl; // #########
        if (!channel)
        {
            if (isValidChannelName(channel_names[i]))
                channel = joinNewChannel(client, channel_names[i], pass);
            else
                client->queueMsg(ERR_BADCHANNAME(channel_names[i]));
        }
        else
            joinExistingChannel(client, channel, pass);
    }
}

Channel *Command::joinNewChannel(Client *client, const std::string &channel_name, const std::string &pass)
{
    Channel *channel = _server->createChannel(channel_name, pass);
    channel->addOperator(client);
    channel->addClient(client);
    client->joinChannel(channel_name);
    sendJoinInfo(client, channel);
    return channel;
}

void Command::joinExistingChannel(Client *client, Channel *channel, const std::string &pass)
{
    if (channel->isMember(client))
        return client->queueMsg(ERR_USERONCHANNEL(client->getNickname(), channel->getName()));
    else if (channel->isInviteOnly() && !channel->isInvitedClient(client))
        return client->queueMsg(ERR_INVITEONLYCHAN(channel->getName()));
    else if (channel->hasPassword() && !channel->checkPasswordEquality(pass))
        return client->queueMsg(ERR_BADCHANNELKEY(channel->getName()));
    else if (channel->isFull())
        return client->queueMsg(ERR_CHANNELISFULL(channel->getName()));

    if (channel->isInviteOnly())
        channel->removeInvitedClient(client);

    channel->addClient(client);
    client->joinChannel(channel->getName());
    // const std::string& message = client->getNickname() + ":" + _args.back() + "\r\n";

    sendJoinInfo(client, channel);

    // channel->globalMessage(client, ":" + client->getNickname() + " JOIN " + channel_names[i] + "\r\n");
}