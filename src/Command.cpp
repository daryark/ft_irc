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
    for (size_t i = 1; i < nick.size(); ++i)
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

    std::cout << "_args.size() = " << _args.size() << std::endl;
    if (_args.size() != 4)
        return _server->send_color(client->getFd(), "461 USER :Not enough parameters", RED);

    const std::string &username = _args[0];
    const std::string &realname = _args[3];
    client->setUserDefault(username, realname);
    client->setRegistered(true);
    _server->send_color(client->getFd(), "001 " + client->getNickname() + " :Welcome to the IRC server", GREEN);
}