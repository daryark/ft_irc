#include "../incl/Command.hpp"

void Command::executePrivmsg(Client *client)
{
    if(!checkPreconditions(client, 3))
        return ;
    const std::set<std::string> targets = splitSet(_args[0], ',');
    for (std::set<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
    {
        if((*it)[0] == '#' || (*it)[0] == '&')
            executePrivmsgToChannel(client, *it);
        else
            executePrivmsgToClient(client, *it);
    }
}

void Command::executePrivmsgToClient(Client* client, const std::string& target)
{
    if(target == client->getNickname())
            return ;
    Client* targetClient = _server->getClientByNickname(target);
    if(!targetClient)
        return client->queueMsg(ERR_NOSUCHNICK(target));
    targetClient->queueMsg(
        MSG(client->getNickname(), client->getUsername(), client->getHostname(), "PRIVMSG", target, _args.back()));
}

void Command::executePrivmsgToChannel(Client* client, const std::string& target)
{
    Channel* channel = _server->getChannelByName(target);
    if(!channel)
        return client->queueMsg(ERR_NOSUCHCHANNEL(target));
    if(!channel->isMember(client))
        return client->queueMsg(ERR_CANNOTSENDTOCHAN(target));
    channel->globalMessage(client,
        MSG(client->getNickname(), client->getUsername(), client->getHostname(), "PRIVMSG", target, _args.back()), false);

}