#include "../incl/Command.hpp"

void Command::executePass(Client *client)
{
    const std::string& nick = client->getSafeNickname();
    if (client->isAuthenticated() || client->isRegistered())
        return client->queueMsg(ERR_ALREADYREGISTRED(nick));
    if (_args.empty())
        return client->queueMsg(ERR_NEEDMOREPARAMS(nick, _command));

    client->authenticate(_args[0] == _server->getPassword());
    if (!client->isAuthenticated())
        return client->queueMsg(ERR_PASSWDMISMATCH(nick));
    welcomeIfRegister(client);
}

void Command::executeNick(Client *client)
{
    bool is_registered = client->isRegistered();
    const std::string& new_nick = _args[0];
    
    if (_args.empty())    
        return client->queueMsg(ERR_NONICKNAMEGIVEN(client->getSafeNickname()));
    if (!isValidNickname())
        return client->queueMsg(ERR_ERRONEUSNICKNAME(client->getSafeNickname(), new_nick));
    
    const std::map<int, Client *> &clients = _server->getClients();
    std::map<int, Client *>::const_iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
    {
        if (it->second->getNickname() == new_nick)
            return client->queueMsg(ERR_NICKNAMEINUSE(new_nick));
    }

    client->setNickname(new_nick);
    if (!is_registered)
        welcomeIfRegister(client);
}    

void Command::executeUser(Client *client)
{
    const std::string& nick = client->getSafeNickname();
    if (client->isRegistered() || !client->getUsername().empty())
        return client->queueMsg(ERR_ALREADYREGISTRED(nick));
    if (_args.size() != 5 || _args[3] != ":")
        return client->queueMsg(ERR_NEEDMOREPARAMS(nick, _command));
        
    const std::string &username = _args[0];
    const std::string &realname = _args[3];
    client->setUser(username, realname);
    welcomeIfRegister(client);
}

bool Command::isValidNickname() const
{
    if (_args.size() != 1)
        return false;

    const std::string &nick = _args[0];    
    if (nick.empty() || nick.size() > 9)
        return false;

    const std::string special = "[]\\`_^{|}"; //(RFC 2812 docs) 
    char c = nick[0];
    if (!isalpha(c) && special.find(c) == std::string::npos)
        return false;   
    for (size_t i = 1; i < nick.size(); ++i)
    {
        c = nick[i];
        if (!isalnum(c) && special.find(c) == std::string::npos && c != '-')
            return false;
    }        
    return true;
}

void Command::welcomeIfRegister (Client* client)
{
    client->setRegistered(!client->getNickname().empty() && !client->getUsername().empty() && client->isAuthenticated());
    if (client->isRegistered())
        client->queueMsg(RPL_WELCOME(client->getNickname(), client->getUsername(), client->getHostname()));
}