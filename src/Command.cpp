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
    (void)args;
}

void Command::executePrivmsg(Client *client)
{
    (void)client;
    // Реализация метода executePrivmsg
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
    if(_args.empty()) {
        _server->send_color(client->getFd(), "461 JOIN :Not enough parameters", RED);
        // client->sendMessage("461 JOIN :Not enough parameters");
        return;
    }
    if(!client->isRegistered()) {
        _server->send_color(client->getFd(), "451 JOIN :Not registered", RED);
        // client->sendMessage("451 JOIN :Not registered");
        return;
    }

    const std::vector<std::string> channelNames = split(_args[0], ',');
    std::vector<std::string> channelsPasswords;
    if(_args.size() == 2) {
        channelsPasswords = split(_args[1], ',');
    }

    for (long unsigned int i = 0; i < channelNames.size(); i++) {
        Channel* channel = _server->getChannelByName(channelNames[i]);
        std::string pass = (i < channelsPasswords.size()) ? channelsPasswords[i] : "";
        if(!channel) {
            channel = _server->createChannel(pass);
            channel->addOperator(client);
        }else {
            if(channel->isInviteOnly() && !channel->isInvitedClient(client)) {
                _server->send_color(client->getFd(), "error", RED);
                // client->sendMessage("error");
            }
            if(channel->hasPassword() && !channel->checkKey(channelsPasswords[i])) { //# changed checkPassword for checkKey only here!
                _server->send_color(client->getFd(), "error", RED);
                // client->sendMessage("error");
            }
            if(channel->isFull()) {
                _server->send_color(client->getFd(), "error", RED);
                // client->sendMessage("error");
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

// void Command::executeQuit(Client* client) {
//     // Реализация метода executeQuit
// }

// void Command::executePart(Client* client) {
//     // Реализация метода executePart
// }

void Command::executeKick(Client *client)
{
    (void)client;
    // Реализация метода executeKick
}

void Command::executeInvite(Client *client)
{
    (void)client;
    // Реализация метода executeInvite
}

void Command::executeTopic(Client *client)
{
    (void)client;
    // Реализация метода executeTopic
}

void Command::executeMode(Client *client)
{
    (void)client;
    // Реализация метода executeMode
}

void Command::executeNick(Client *client)
{
    if (_args.empty())
    {
        _server->send_color(client->getFd(), "431 :No nickname given", RED);
        // client->sendMessage("431 :No nickname given");
        return;
    }

    std::string nickname = _args[0];
    if (nickname.empty() || nickname.find(' ') != std::string::npos)
    {
        _server->send_color(client->getFd(), "432 " + nickname + " :Erroneous nickname", RED);
        // client->sendMessage("432 " + nickname + " :Erroneous nickname");
        return;
    }
    //? check working this part with const iterator
    const std::map<int, Client *> &clients = _server->getClients();
    const std::map<int, Client *>::const_iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->second->getNickname() == nickname)
        {
            _server->send_color(client->getFd(), "433: Nickname is already in use.", RED);
            // client->sendMessage("433: Nickname is already in use.");
            return;
        }
    }
    client->setNickname(nickname);
    _server->send_color(client->getFd(), "001 " + nickname + " :Welcome to the IRC server", RED);
    // client->sendMessage("001 " + nickname + " :Welcome to the IRC server");

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
        _server->send_color(client->getFd(), "461: PASS Not enough parameters supplied for the command.\n", RED);
        // client->sendMessage("461: PASS Not enough parameters supplied for the command.\n");
        return;
    }

    if (client->isAuthenticated())
    {
        _server->send_color(client->getFd(), "462: Already registered — cannot register again.", RED);
        // client->sendMessage("462: Already registered — cannot register again.");
    }

    client->authenticate(_args[0] == _server->getPassword());
    if (!client->isAuthenticated())
    {
        _server->send_color(client->getFd(), "464: Password mismatch — wrong password sent with PASS.", RED);
        // client->sendMessage("464: Password mismatch — wrong password sent with PASS.");
    }
}
void Command::executeUser(Client *client)
{
    if (_args.size() != 4)
    {
        _server->send_color(client->getFd(), "461 USER :Not enough parameters", RED);
        // client->sendMessage("461 USER :Not enough parameters");
        return;
    }

    if (client->isRegistered())
    {
        _server->send_color(client->getFd(), "462 :You may not reregister", RED);
        // client->sendMessage("462 :You may not reregister");
        return;
    }

    const std::string &username = _args[0];
    const std::string &realname = _args[3];
    client->setUserDefault(username, realname);

    if (!client->getNickname().empty() && !client->isRegistered())
    {
        client->setRegistered(true);
        _server->send_color(client->getFd(), "001 " + client->getNickname() + " :Welcome to the IRC server", RED);
        // client->sendMessage("001 " + client->getNickname() + " :Welcome to the IRC server");
    }
}