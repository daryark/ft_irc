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
    _commandMap["INVITE"] = &Command::executeInvite;
    _commandMap["TOPIC"] = &Command::executeTopic;
    _commandMap["MODE"] = &Command::executeMode;
    _commandMap["QUIT"] = &Command::executeQuit;
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
    std::cout << "[" << _command << "]";

    std::map<std::string, CommandHandler>::iterator it = _commandMap.find(_command);
    if (it != _commandMap.end())
    {
        CommandHandler handle = it->second;
        (this->*handle)(client);
    }
    else
        client->queueMsg("Command not found\r\n");
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

    // Отправить клиенту:
    //1. подтверждение JOIN,
    //2.TOPIC (если есть),
    //3.NAMES (список участников).

// void Command::executePong(Client* client) {
//     // Реализация метода executePong
// }

void Command::executeQuit(Client* client) {
    if (_args.size() == 0)
        std::cout << BG_I_BLUE << client->getNickname() << " disconnected(SENT TO EVERYONE WITH POLLOUT)" << RE << "; ";
    else
        std::cout << BG_WHITE << "send this quit msg to everyone(SENT TO EVERYONE WITH POLLOUT) ALL THE _args ARR: " << BG_YELLOW << _args[0] << RE << std::endl;
    _server->disconnectClient(client->getFd());
    }

// void Command::executePart(Client* client) {
//     // Реализация метода executePart
// }

void Command::executeKick(Client *client)
{
    // if(!client->isRegistered())
    //     return client->queueMsg("451 JOIN :Not registered");
    // Реализация метода executeKick
    if(_args.size() < 2)
        return client->queueMsg("461 KICK :Not enough parameters");
    
    const std::string &channelName = _args[0];
    const std::string &targetNick = _args[1];
    std::string comment = (_args.size() >= 3) ? _args[2] : "No reason";

    Channel* channel = _server->getChannelByName(channelName);
    if(!channel)
        return client->queueMsg("403 " + channelName + " :No such channel");
    if(!channel->isOperator(client))
        return client->queueMsg("482 " + channelName + " :You're not channel operator");
    Client* targetClient = _server->getClientByNickname(targetNick);
    if(!targetClient)
        return client->queueMsg("401 " + targetNick + " :No such nick");
    if(!channel->isMember(targetClient))
        return client->queueMsg("441 " + targetNick + " " + channelName + " :They aren't on that channel");
    channel->removeClient(targetClient);
    targetClient->removeChannel(channelName);
    channel->globalMessage(client, "KICK " + channelName + " " + targetNick + " :" + comment);
    // Notify target client about being kicked
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
    // if(!client->isRegistered())
    //     return client->queueMsg("451 JOIN :Not registered");
    // Реализация метода executeTopic
    if(_args.empty())
        return client->queueMsg("461 TOPIC :Not enough parameters");
    
    const std::string &channelName = _args[0];
    Channel* channel = _server->getChannelByName(channelName);
    if(!channel)
        return client->queueMsg("403 " + channelName + " :No such channel");
    if(!channel->isMember(client))
        return client->queueMsg("442 " + channelName + " :You're not on that channel");
    if(_args.size() == 1) {
        if(channel->getTopic().empty())
            return client->queueMsg("331 " + channelName + " :No topic is set");
        else
            return client->queueMsg("332 " + channelName + " :" + channel->getTopic());
    } else {
        if(!channel->isOperator(client) && channel->getTopic().empty())
            return client->queueMsg("482 " + channelName + " :You're not channel operator");
        
        
        const std::string &newTopic = _args[1];
        channel->setTopic(newTopic);
        channel->globalMessage(client, "TOPIC " + channelName + " :" + newTopic);
    }
}

void Command::executeMode(Client *client)
{
    if(!client->isRegistered())
        return client->queueMsg("451 JOIN :Not registered");
    // Реализация метода executeMode

}