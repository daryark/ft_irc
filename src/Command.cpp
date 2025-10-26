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
}

Command::~Command()
{
}

void Command::executeCommand(Client *client)
{
    std::map<std::string, CommandHandler>::iterator it = _commandMap.find(_command);
    if (it != _commandMap.end())
    {
        CommandHandler handle = it->second;
        (this->*handle)(client);
    }
    else
    _server->send_color(client->getFd(), "Command not found", RED);
}

void Command::executePrivmsg(Client *client)
{
    //    Command: PRIVMSG
    //    Parameters: <msgtarget>{,<msgtarget>} :<text to be sent>
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    // if (_args.size() < 2)
    // {
    //     if (_args.size() == 1)
    //     {
    //         if (_args[0].length() < 10)
    //             _server->send_color(client->getFd(), "ERR_NOTEXTTOSEND");
    //         else
    //             _server->send_color(client->getFd(), "ERR_NORECIPIENT");
    //     }
    // }
    // // for (std::vector<std::string>::iterator i = 0; i < (_args.size() - 1); i++)
    // // {
    // //     if (i == 0)
    // //         _server->send_color(client->getFd(), "no target to send msg to", RED);
    // //     else
    // //         _server->send_color(client->getFd(), "send msg", BG_WHITE);
    // //     break ;
    // // }

    // std::cout << BG_GREEN << _args.size() << RE << std::endl;
    if(_args.size() < 2)
        return _server->send_color(client->getFd(), "461 PRIVMSG :Not enough parameters", RED);
    
    const std::string &target = _args[0];
    const std::string &message = _args[1];

    //? need to test, but probably unreal becauce parsing splits by space
    if(target.empty())
        return _server->send_color(client->getFd(), "411 :No recipient given (PRIVMSG)", RED);
    if(message.empty())
        return _server->send_color(client->getFd(), "412 :No text to send", RED);
    

    if(target[0] == '#' || target[0] == '&') {
        Channel* channel = _server->getChannelByName(target);
        if(!channel)
            return _server->send_color(client->getFd(), "403 " + target + " :No such channel", RED);
        if(!channel->isMember(client))
            return _server->send_color(client->getFd(), "404 " + target + " :Cannot send to channel", RED);

        channel->globalMassage(target + " :" + message);
        // for each member in channel->getMembers()
        //     _server->send_color(member->getFd(), "PRIVMSG " + target + " :" + message, GREEN);
    } else {
        Client* targetClient = _server->getClientByNickname(target);
        if(!targetClient)
            return _server->send_color(client->getFd(), "401 " + target + " :No such nick", RED);
        // send(targetClient->getFd(), message.c_str(), message.length(), 0);         
        _server->send_color(targetClient->getFd(), "PRIVMSG " + target + " :" + message, GREEN);
    }
}

//#out of class, move to the helper file?!
std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Command::executeJoin(Client *client)
{
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    if(_args.empty())
        return _server->send_color(client->getFd(), "461 JOIN :Not enough parameters", RED);

    const std::vector<std::string> channelNames = split(_args[0], ',');
    std::vector<std::string> channelsPasswords;
    if(_args.size() == 2)
        channelsPasswords = split(_args[1], ',');

    for (long unsigned int i = 0; i < channelNames.size(); i++) {
        Channel* channel = _server->getChannelByName(channelNames[i]);
        std::string pass = (i < channelsPasswords.size()) ? channelsPasswords[i] : "";
        if(!channel) {
            channel = _server->createChannel(pass);
            channel->addOperator(client);
        } else {
            if(channel->isInviteOnly() && !channel->isInvitedClient(client)) {
                _server->send_color(client->getFd(), "error", RED);
            }
            if(channel->hasPassword() && !channel->checkKey(channelsPasswords[i])) { //# changed checkPassword for checkKey only here!
                _server->send_color(client->getFd(), "error", RED);
            }
            if(channel->isFull()) {
                _server->send_color(client->getFd(), "error", RED);
            }
        }
        channel->addClient(client);
    }


    // Отправить клиенту:
    //1. подтверждение JOIN,
    //2.TOPIC (если есть),
    //3.NAMES (список участников).

}

// void Command::executePong(Client* client) {
//     // Реализация метода executePong
// }

void Command::executeQuit(Client* client) {
    if (_args.size() == 0)
        std::cout << BG_I_BLUE << client->getNickname() << " disconnected(SENT TO EVERYONE WITH POLLOUT)" << RE << "; ";
    else
        std::cout << BG_WHITE << "send this quit msg to everyone(SENT TO EVERYONE WITH POLLOUT) ALL THE _args ARR: " << BG_YELLOW << _args[0] << RE << std::endl;
    _server->disconnect_client(client->getFd());
    shutdown(client->getFd(), SHUT_RDWR);
    }

// void Command::executePart(Client* client) {
//     // Реализация метода executePart
// }

void Command::executeKick(Client *client)
{
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
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
    channel->globalMassage("KICK " + channelName + " " + targetNick + " :" + comment);
    // Notify target client about being kicked
}

void Command::executeInvite(Client *client)
{
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
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
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
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
        channel->globalMassage("TOPIC " + channelName + " :" + newTopic);
    }
}

void Command::executeMode(Client *client)
{
    if(!client->isRegistered())
        return _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
    // Реализация метода executeMode

}

void Command::executeNick(Client *client)
{
    if(!client->isAuthenticated())
        return _server->send_color(client->getFd(), "XXX :Register on the server first!!!!!!!!", RED);
    if (_args.empty())
        return _server->send_color(client->getFd(), "431 :No nickname given", RED);
    if (!isValidNickname())
        return _server->send_color(client->getFd(), "432 :Erroneous nickname", RED);

    std::string nickname = _args[0];
    if (nickname.empty() || nickname.find(' ') != std::string::npos) //!never goes into this if, what does it do??
        return _server->send_color(client->getFd(), "432 " + nickname + " :Erroneous nickname", RED);
    
    //? check working this part with const iterator
    const std::map<int, Client *> &clients = _server->getClients();
    std::map<int, Client *>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
    {
        if (it->second->getNickname() == nickname)
            return _server->send_color(client->getFd(), "433: Nickname is already in use.", RED);
    }
    client->setNickname(_args[0]);
    // if(!client->getUsername().empty() && !client->isRegistered()) {
    //     client->setRegistered(true);
    //     _server->send_color(client->getFd(), "001 " + nickname + PR_WELCOME, B_GREEN);
    //     // _server->send_color(client->getFd(), PR_USAGE, B_WHITE);
    // }
}

bool Command::isValidNickname() const
{
    // reject if args array has more than 1 element
    if (_args.size() != 1)
        return false;

    const std::string &nick = _args[0];
    if (nick.empty() || nick.size() > 9)
        return false;

    // allowed special chars (RFC 2812)
    const std::string special = "[]\\`_^{|}";

    // first char: letter or special
    char c = nick[0];
    if (!isalpha(c) && special.find(c) == std::string::npos)
        return false;

    // remaining chars: letter / digit / special / '-'
    for (size_t i = 1; i < nick.size(); ++i)//?! ++i or i++ ??
    {
        c = nick[i];
        if (!isalnum(c) && special.find(c) == std::string::npos && c != '-')
            return false;
    }
    return true;
}

void Command::executePass(Client *client)
{
    if (client->isAuthenticated())
        return _server->send_color(client->getFd(), "462: Already registered — cannot register again.", RED);
    if (_args.empty())
        return _server->send_color(client->getFd(), "461: PASS Not enough parameters supplied for the command.\n", RED);

    client->authenticate(_args[0] == _server->getPassword());
    if (!client->isAuthenticated())
        _server->send_color(client->getFd(), "464: Password mismatch — wrong password sent with PASS.", RED);
}

void Command::executeUser(Client *client)
{
    if (client->isRegistered())
        return _server->send_color(client->getFd(), "462: Already registered — cannot register again.", RED);
    else if (!client->isAuthenticated())
        return _server->send_color(client->getFd(), "XXX :Register on the server first!!!!!!!!", RED);
    else if (client->getNickname() == "")
        return _server->send_color(client->getFd(), "431 :No nickname given", RED);

    // std::cout << "_args.size() = " << _args.size() << std::endl;
    if (_args.size() != 5 || _args[3] != ":")
        return _server->send_color(client->getFd(), "461 USER :Not enough parameters", RED);

    const std::string &username = _args[0];
    const std::string &realname = _args[3];
    client->setUserDefault(username, realname);

    if(!client->getNickname().empty() && !client->isRegistered()){
        client->setRegistered(true);
        _server->send_color(client->getFd(), "001 " + client->getNickname() + PR_WELCOME, B_GREEN);
    }
    // client->setRegistered(true);//?
    // _server->send_color(client->getFd(), PR_USAGE, B_WHITE);
}