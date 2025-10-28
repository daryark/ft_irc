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
    std::cout << "[" << _command << "]" << std::endl;

    std::map<std::string, CommandHandler>::iterator it = _commandMap.find(_command);
    if (it != _commandMap.end())
    {
        CommandHandler handle = it->second;
        (this->*handle)(client);
    }
    else
    _server->send_color(client->getFd(), "Command not found", RED);
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
    return _server->send_color(client->getFd(), "461 allMC :Not enough parameters", RED);
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
    _server->disconnect_client2(client->getFd());
    }

// void Command::executePart(Client* client) {
//     // Реализация метода executePart
// }

void Command::executeKick(Client *client)
{
    // if(!client->isRegistered())
    //     return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    // Реализация метода executeKick
    if(_args.size() < 2)
        return _server->send_color(client->getFd(), "461 KICK :Not enough parameters", RED);
    
    const std::string &channelName = _args[0];
    const std::string &targetNick = _args[1];
    std::string comment = (_args.size() >= 3) ? _args[2] : "No reason";

    Channel* channel = _server->getChannelByName(channelName);
    if(!channel)
        return _server->send_color(client->getFd(), "403 " + channelName + " :No such channel", RED);
    if(!channel->isOperator(client))
        return _server->send_color(client->getFd(), "482 " + channelName + " :You're not channel operator", RED);
    Client* targetClient = _server->getClientByNickname(targetNick);
    if(!targetClient)
        return _server->send_color(client->getFd(), "401 " + targetNick + " :No such nick", RED);
    if(!channel->isMember(targetClient))
        return _server->send_color(client->getFd(), "441 " + targetNick + " " + channelName + " :They aren't on that channel", RED);
    channel->removeClient(targetClient);
    targetClient->removeChannel(channelName);
    channel->globalMessage(client, "KICK " + channelName + " " + targetNick + " :" + comment);
    // Notify target client about being kicked
}

void Command::executeInvite(Client *client)
{
    // if(!client->isRegistered())
    //     return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    // Реализация метода executeInvite

    if(_args.size() < 2)
        return _server->send_color(client->getFd(), "461 INVITE :Not enough parameters", RED);
    
    const std::string &targetNick = _args[0];
    const std::string &channelName = _args[1];

    Channel* channel = _server->getChannelByName(channelName);
    if(!channel)
        return _server->send_color(client->getFd(), "403 " + channelName + " :No such channel", RED);
    
    if(!channel->isMember(client))
        return _server->send_color(client->getFd(), "442 " + channelName + " :You're not on that channel", RED);
    
    if(!channel->isOperator(client))
        return _server->send_color(client->getFd(), "482 " + channelName + " :You're not channel operator", RED);
    
    Client* targetClient = _server->getClientByNickname(targetNick);
    if(!targetClient)
        return _server->send_color(client->getFd(), "401 " + targetNick + " :No such nick", RED);
    
    if(channel->isMember(targetClient))
        return _server->send_color(client->getFd(), "443 " + targetNick + " " + channelName + " :is already on channel", RED);
    
    channel->addClient(targetClient);
    targetClient->joinChannel(channelName);

}

//*TOPIC <channel> [: [<topic>]]
void Command::executeTopic(Client *client)
{
    // if(!client->isRegistered())
    //     return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    // Реализация метода executeTopic
    if(_args.empty())
        return _server->send_color(client->getFd(), "461 TOPIC :Not enough parameters", RED);
    
    const std::string &channelName = _args[0];
    Channel* channel = _server->getChannelByName(channelName);
    if(!channel)
        return _server->send_color(client->getFd(), "403 " + channelName + " :No such channel", RED);
    if(!channel->isMember(client))
        return _server->send_color(client->getFd(), "442 " + channelName + " :You're not on that channel", RED);
    if(_args.size() == 1) {
        if(channel->getTopic().empty())
            return _server->send_color(client->getFd(), "331 " + channelName + " :No topic is set", YELLOW);
        else
            return _server->send_color(client->getFd(), "332 " + channelName + " :" + channel->getTopic(), GREEN);
    } else {
        if(!channel->isOperator(client) && channel->getTopic().empty())
            return _server->send_color(client->getFd(), "482 " + channelName + " :You're not channel operator", RED);
        
        
        const std::string &newTopic = _args[1];
        channel->setTopic(newTopic);
        channel->globalMessage(client, "TOPIC " + channelName + " :" + newTopic);
    }
}

void Command::executeMode(Client *client)
{
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    // Реализация метода executeMode

}