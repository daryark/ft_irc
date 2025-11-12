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
    std::cout << "[" << _command << "]\n"; //###########

    std::map<std::string, CommandHandler>::iterator it = _commandMap.find(_command);
    if (it != _commandMap.end())
    {
        CommandHandler handle = it->second;
        (this->*handle)(client);
    }
    else
        client->queueMsg(ERR_UNKNOWNCOMMAND(_command));
}

void Command::executeAllInfo(Client* client) 
{
    client->printInfo();
}

void Command::executeAllClients(Client* client) 
{
    (void)client;
    PrintAllClients(*_server);
}

void Command::executeAllChannel(Client* client) 
{
    (void)client;
    PrintAllChannels(*_server);
}

void Command::executeAllMembersInChannel(Client* client)
{
    (void)client;
    if(_args.empty())
        return client->queueMsg("461 allMC :Not enough parameters");
    PrintMembersInChannel(*_server, _args[0]);
}

// void Command::executePong(Client* client) {
//     // Реализация метода executePong
// }

//!if this member was an op in the channels, act the same way as in PART,KICK
void Command::executeQuit(Client* client) {
    if (_args.size() == 0)
        std::cout << BG_I_BLUE << client->getNickname() << " disconnected(SENT TO EVERYONE WITH POLLOUT)" << RE << "; ";
    else
        std::cout << BG_WHITE << "send this quit msg to everyone(SENT TO EVERYONE WITH POLLOUT) ALL THE _args ARR: " << BG_YELLOW << _args[0] << RE << std::endl;
    _server->disconnectClient(client->getFd());
    }

void Command::executeInvite(Client *client)
{
    // if(!client->isRegistered())
    //     return client->queueMsg("451 JOIN :Not registered");
    // Реализация метода executeInvite

    if(_args.size() < 2)
        return client->queueMsg("461 INVITE :Not enough parameters");
    
    const std::string &targetNick = _args[0];
    const std::string &channelName = _args[1];

    Channel* channel = _server->getChannelByName(channelName);
    if(!channel)
        return client->queueMsg("403 " + channelName + " :No such channel");
    
    if(!channel->isMember(client))
        return client->queueMsg("442 " + channelName + " :You're not on that channel");
    
    if(!channel->isOperator(client))
        return client->queueMsg("482 " + channelName + " :You're not channel operator");
    
    Client* targetClient = _server->getClientByNickname(targetNick);
    if(!targetClient)
        return client->queueMsg("401 " + targetNick + " :No such nick");
    
    if(channel->isMember(targetClient))
        return client->queueMsg("443 " + targetNick + " " + channelName + " :is already on channel");
    
    channel->addClient(targetClient);
    targetClient->joinChannel(channelName);

}

//*TOPIC <channel> [: [<topic>]]
void Command::executeTopic(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg(ERR_NOTREGISTERED(client->getSafeNickname(), "TOPIC"));
    if(_args.empty())
        return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "TOPIC"));
    
    const std::string &channelName = _args[0];
    Channel* channel = _server->getChannelByName(channelName);
    if(!channel)
        return client->queueMsg(ERR_NOSUCHCHANNEL(channelName));
    if(!channel->isMember(client))
        return client->queueMsg(ERR_NOTONCHANNEL(channelName));
    if(_args.size() == 1) {
        if(channel->getTopic().empty())
            return client->queueMsg(RPL_NOTOPIC(channelName));
        else
            return client->queueMsg(RPL_TOPIC(channelName, channel->getTopic()));
    } else
    {
        if (_args[1] != ":")
            return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "TOPIC"));
        //!if -t (flag) - then only operator can change the topic
        // if(!channel->isOperator(client) && channel->getTopic().empty())
        //     return client->queueMsg(ERR_CHANOPRIVSNEEDED(channelName));
        const std::string &newTopic = _args.size() == 2 ? "" : _args[2];
        channel->setTopic(newTopic);
        channel->globalMessage(client,
        MSG(client->getNickname(),client->getUsername(), client->getHostname(),
        "TOPIC", channelName, newTopic));
    }
}

void Command::executeMode(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg("451 JOIN :Not registered");
    // Реализация метода executeMode

    if(_args.empty())
        return client->queueMsg("461" " MODE :Not enough parameters");
    
    const std::string &channelName = _args[0];
    Channel* channel = _server->getChannelByName(channelName);

    if(!channel)
        return client->queueMsg("403 " + channelName + " :No such channel");

    //only made #channel
    if(_args.size() == 1) {
        std::string modes = "+";
        if(channel->hasPassword())
            modes += "k";
        if(channel->isInviteOnly())
            modes += "i";
        if(channel->isTopicSetByOperator())
            modes += "t";
        if(channel->hasLimit())
            modes += "l";
        return client->queueMsg("324 " + channelName + " " + modes);
    }

    // check is operator
    if(!channel->isOperator(client))
        return client->queueMsg("482 " + channelName + " :You're not channel operator");

    bool adding = true;

    size_t argIndex = 2;
    std::string modeChanges;
    std::string modes;

    for (size_t i = 0; i < _args.size(); i++)
    {
        const std::string &modeStr = _args[i];
        for (size_t j = 0; j < modeStr.size(); j++)
        {
            char mode = modeStr[j];
            if (mode == '+')
                adding = true;
            else if (mode == '-')
                adding = false;
            
            switch (mode)
            {
                case 'i':
                    channel->setInviteOnly(adding);
                    modeChanges += (adding ? "+i " : "-i ");
                    break;
                case 't':
                    channel->setTopicSetByOperator(adding);
                    modeChanges += (adding ? "+t " : "-t ");
                    break;
                case 'k':
                    if (adding)
                    {
                        if (argIndex >= _args.size())
                            return client->queueMsg("461 MODE :Not enough parameters for +k");
                        const std::string &newPass = _args[argIndex++];
                        channel->setPassword(newPass);
                        modeChanges += "+k ";
                        modes += " " + newPass;
                    }
                    else
                    {
                        channel->setPassword("");
                        modeChanges += "-k ";
                    }
                    break;
                case 'o':
                    if(argIndex >= _args.size())
                        return client->queueMsg("461 MODE :Not enough parameters for o");
                    {
                        std::string nick = _args[argIndex++];
                        Client *target = _server->getClientByNickname(nick);
                        if (!target)
                        {
                            client->queueMsg("401 " + nick + " :No such nick");
                            continue;
                        }
                        if (!channel->isMember(target))
                        {
                            client->queueMsg("441 " + nick + " " + channelName + " :They aren't on that channel");
                            continue;
                        }
                        if (adding)
                            channel->addOperator(target);
                        else
                            channel->removeOperator(target);
                        modeChanges += (adding ? "+o" : "-o");
                        modes += " " + nick;
                    }
                    break;
                case 'l':
                    if (adding)
                    {
                        if (argIndex >= _args.size())
                            return client->queueMsg("461 MODE :Not enough parameters for +l");
                        int maxClients = std::stoi(_args[argIndex++]);
                        channel->setMaxClients(maxClients);
                        modeChanges += "+l ";
                        modes += " " + std::to_string(maxClients);
                    }
                    else
                    {
                        channel->setMaxClients(0);
                        modeChanges += "-l ";
                    }
                    break;
           default:
           client->queueMsg("472 " + std::string(1, mode) + " :is unknown mode char to me");
           break;

        }
    }

    if(!modeChanges.empty())
    {
        modeChanges.pop_back(); // remove trailing space
        channel->globalMessage(client, "MODE " + channelName + " " + modeChanges + modes);
    }
}
}
