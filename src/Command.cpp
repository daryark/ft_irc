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

void Command::executeJoin(Client *client)
{
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
    const std::map<int, Client *> &clients = _server->getCliends();
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
        return;
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
        client->sendMessage("");
        return;
    }
    
}