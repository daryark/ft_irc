#include "../incl/Command.hpp"

std::map<std::string, Command::CommandHandler> Command::_commandMap;

Command::Command(Server *server, const std::string &command, const std::vector<std::string> &args)
    : _command(command), _args(args)
{
    _server = server;
    if (_commandMap.empty())
        initCommandMap();
}

void Command::initCommandMap()
{
    _commandMap["USER"] = &Command::executeUser;
    _commandMap["NICK"] = &Command::executeNick;
    _commandMap["PASS"] = &Command::executePass;
    _commandMap["JOIN"] = &Command::executeJoin;
    _commandMap["PRIVMSG"] = &Command::executePrivmsg;
    _commandMap["KICK"] = &Command::executeKick;
    _commandMap["PART"] = &Command::executePart;
    _commandMap["INVITE"] = &Command::executeInvite;
    _commandMap["TOPIC"] = &Command::executeTopic;
    _commandMap["MODE"] = &Command::executeMode;
    _commandMap["QUIT"] = &Command::executeQuit;
    _commandMap["INVITE"] = &Command::executeInvite;

    _commandMap["allClients"] = &Command::executeAllClients;
    _commandMap["allChannel"] = &Command::executeAllChannel;
    _commandMap["allMC"] = &Command::executeAllMembersInChannel;
    _commandMap["info"] = &Command::executeAllInfo;
}

Command::~Command()
{
}

void Command::executeCommand(Client *client)
{
    std::cout << "[" << _command << "]\n"; // ###########

    std::map<std::string, CommandHandler>::iterator it = _commandMap.find(_command);
    if (it != _commandMap.end())
    {
        CommandHandler handle = it->second;
        (this->*handle)(client);
    }
    else
        client->queueMsg(ERR_UNKNOWNCOMMAND(_command));
}

void Command::executeAllInfo(Client *client)
{
    client->printInfo();
}

void Command::executeAllClients(Client *client)
{
    (void)client;
    PrintAllClients(*_server);
}

void Command::executeAllChannel(Client *client)
{
    (void)client;
    PrintAllChannels(*_server);
}

void Command::executeAllMembersInChannel(Client *client)
{
    (void)client;
    if (_args.empty())
        return client->queueMsg("461 allMC :Not enough parameters");
    PrintMembersInChannel(*_server, _args[0]);
}

bool Command::checkPreconditions(Client *client, size_t min_args_size)
{
    if (!client->isRegistered())
    {
        client->queueMsg(ERR_NOTREGISTERED(client->getSafeNickname(), _command));
        return false;
    }
    else if (_args.size() < min_args_size)
    {
        client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), _command));
        return false;
    }
    return true;
}

// void Command::executePong(Client* client) {
//     // Реализация метода executePong
// }

void Command::executeQuit(Client *client)
{
    if (!checkPreconditions(client, 0))
        return ;
    leaveChannels(client, client->getJoinedChannels());
    // _server->disconnectClient(client->getFd());
    _server->disconnectClient(1111);
}

void Command::executeInvite(Client *client)
{
    if (!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getSafeNickname(), "INVITE"));
    // Реализация метода executeInvite

    if (_args.size() < 2)
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "INVITE"));

    const std::string &targetNick = _args[0];
    const std::string &channelName = _args[1];

    Channel *channel = _server->getChannelByName(channelName);
    if(!channel)
        return client->queueMsg(ERR_NOSUCHCHANNEL(channelName));

    if(!channel->isMember(client))
        return client->queueMsg(ERR_NOTONCHANNEL(channelName));

    if(!channel->isOperator(client))
        return client->queueMsg(ERR_CHANOPRIVSNEEDED(channelName));

    Client *targetClient = _server->getClientByNickname(targetNick);
    if(!targetClient)
        return client->queueMsg(ERR_NOSUCHNICK(targetNick));

    if(channel->isMember(targetClient))
        return client->queueMsg(ERR_USERONCHANNEL(targetNick, channelName));

    // channel->addClient(targetClient);
    // targetClient->joinChannel(channelName);
    channel->addInvitedClient(targetClient);
    targetClient->queueMsg(RPL_INVITING(client->getNickname(), targetClient->getNickname(), channel->getName()));
}

//*TOPIC <channel> [: [<topic>]]
void Command::executeTopic(Client *client)
{
    if (!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getSafeNickname(), "TOPIC"));
    if (_args.empty())
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "TOPIC"));

    const std::string &channelName = _args[0];
    Channel *channel = _server->getChannelByName(channelName);
    if (!channel)
        return client->queueMsg(ERR_NOSUCHCHANNEL(channelName));
    if (!channel->isMember(client))
        return client->queueMsg(ERR_NOTONCHANNEL(channelName));
    if (_args.size() == 1)
    {
        if (channel->getTopic().empty())
            return client->queueMsg(RPL_NOTOPIC(channelName));
        else
            return client->queueMsg(RPL_TOPIC(channelName, channel->getTopic()));
    }
    else
    {
        if (_args[1] != ":")
            return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "TOPIC"));
        //! if -t (flag) - then only operator can change the topic
        if (!channel->isOperator(client) && channel->isTopicSetByOperator())
            return client->queueMsg(ERR_CHANOPRIVSNEEDED(channelName));
        const std::string &newTopic = _args.size() == 2 ? "" : _args[2];
        channel->setTopic(newTopic);
        channel->globalMessage(client,
        MSG(client->getNickname(),client->getUsername(), client->getHostname(),
        "TOPIC", channelName, newTopic), true);
    }
}
