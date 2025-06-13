#include "../incl/Command.hpp"

std::map<std::string, Command::CommandHandler> Command::_commandMap;

Command::Command(Server *server, const std::string &command, const std::vector<std::string> &args)
    : _server(server), _command(command), _args(args)
{
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
}

Command::~Command()
{
}

void Command::executeCommand(Client *client, const std::vector<std::string> &args)
{
    std::map<std::string, CommandHandler>::iterator it = _commandMap.find(_command);
    if (it != _commandMap.end())
    {
        CommandHandler handel = it->second;
        (this->*handel)(client);
    }
    else
    {
        std::cout << "Command not found" << std::endl;
    }
}

void Command::executePrivmsg(Client *client)
{
    // Реализация метода executePrivmsg
}

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
    if(_args.empty()) {
        client->sendMessage("461 JOIN :Not enough parameters");
        return;
    }
    if(!client->isRegistered()) {
        client->sendMessage("451 JOIN :Not registered");
        return;
    }

    const std::vector<std::string> channelNames = split(_args[0], ',');
    std::vector<std::string> channelsPasswords;
    if(_args.size() == 2) {
        channelsPasswords = split(_args[1], ',');
    }

    for (int i = 0; i < channelNames.size(); i++) {
        Channel* channel = _server->getChannelByName(channelNames[i]);

        if(!channel) {
            channel = _server->createChannel(channelNames[i]);
            channel->addOperator(client);
        }else {
            if(channel->isInviteOnly() && !channel->isInviteOnly()) {

            }

        }
    }








    // if()
    // const std::map<int, Client *> &clients = _server->getCliends();

    // if(_max_clients != -1)
    // Реализация метода executeJoin
}

// void Command::executePong(Client* client) {
//     // Реализация метода executePong
// }

// void Command::executeQuit(Client* client) {
//     // Реализация метода executeQuit
// }

// void Command::executePart(Client* client) {
//     // Реализация метода executePart
// }

void Command::executeKick(Client *client)
{
    // Реализация метода executeKick
}

void Command::executeInvite(Client *client)
{
    // Реализация метода executeInvite
}

void Command::executeTopic(Client *client)
{
    // Реализация метода executeTopic
}

void Command::executeMode(Client *client)
{
    // Реализация метода executeMode
}

void Command::executeNick(Client *client)
{
    if (_args.empty())
    {
        client->sendMessage("431 :No nickname given");
        return;
    }

    std::string nickname = _args[0];
    if (nickname.empty() || nickname.find(' ') != std::string::npos)
    {
        client->sendMessage("432 " + nickname + " :Erroneous nickname");
        return;
    }
    //? check working this part with const iterator
    const std::map<int, Client *> &clients = _server->getClients();
    const std::map<int, Client *>::const_iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->second->getNickname() == nickname)
        {
            client->sendMessage("433: Nickname is already in use.");
            return;
        }
    }
    client->setNickname(nickname);
    client->sendMessage("001 " + nickname + " :Welcome to the IRC server");

    //?
    // if (!client->getUsername().empty() && !client->isRegistered())
    // {
    //     client->setRegistered(true);
    //     client->sendMessage("001 " + nickname + " :Welcome to the IRC server");
    // }
}

void Command::executePass(Client *client)
{
    if (_args.empty())
    {
        client->sendMessage("461: PASS Not enough parameters supplied for the command.\n");
        return;
    }

    if (client->isAuthenticated())
    {
        client->sendMessage("462: Already registered — cannot register again.");
    }

    client->authenticate(_args[0] == _server->getPassword());
    if (!client->isAuthenticated())
    {
        client->sendMessage("464: Password mismatch — wrong password sent with PASS.");
        return;
    }
}
void Command::executeUser(Client *client)
{
    if (_args.size() != 4)
    {
        client->sendMessage("461 USER :Not enough parameters");
        return;
    }

    if (client->isRegistered())
    {
        client->sendMessage("462 :You may not reregister");
        return;
    }

    const std::string &username = _args[0];
    const std::string &realname = _args[3];
    client->setUserDefault(username, realname);

    if (!client->getNickname().empty() && !client->isRegistered())
    {
        client->setRegistered(true);
        client->sendMessage("001 " + client->getNickname() + " :Welcome to the IRC server");
    }
}