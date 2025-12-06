#include "../incl/Client.hpp"
#include "../incl/Server.hpp"
#include "../incl/Command.hpp"

void Command::executeAllInfo(Client *client)
{
    client->printInfo();
}

void Command::executeAllClients(Client *client)
{
    (void)client;
    const std::map<int, Client*>& clients = _server->getClients();
    std::cout << "----- Clients List -----" << std::endl;

    std::map<int, Client*>::const_iterator it = clients.begin();
    for (; it != clients.end(); ++it)
    {
        Client *client = it->second;
        std::cout << "FD: " << client->getFd()
                  << ", Nickname: " << client->getNickname()
                  << ", Authenticated: " << (client->isAuthenticated() ? "Yes" : "No")
                  << ", Registered: " << (client->isRegistered() ? "Yes" : "No")
                  << std::endl;
    }

    std::cout << "------------------------" << std::endl;
}

void Command::executeAllChannel(Client *client)
{
    (void)client;
    const std::map<std::string, Channel*>& channels = _server->getChannel();

    std::cout << "Size of channels map: " << channels.size() << std::endl;
    std::cout << "----- Channels List -----" << std::endl;

    std::map<std::string, Channel*>::const_iterator it = channels.begin();
    for (; it != channels.end(); ++it)
    {
        Channel *channel = it->second;
        std::cout << "Channel Name: " << channel->getName()
                  << ", Topic: " << channel->getTopic()
                  << ", Number of Clients: " << channel->getSize()
                  << std::endl;
    }

    std::cout << "-------------------------" << std::endl;

}

void Command::executeAllMembersInChannel(Client *client)
{
    (void)client;
    if (_args.empty())
    {
        client->queueMsg("461 allMC :Not enough parameters");
        return;
    }
    std::cout << "-----------------ALL------------------" << std::endl;

    const std::string channel_name = _args[0];
    Channel *channel = _server->getChannelByName(channel_name);
    if (!channel)
    {
        std::cout << "Channel " << channel_name << " does not exist." << std::endl;
        return;
    }

    // Print members
    const std::map<int, Client*>& clients = _server->getClients();
    std::map<int, Client*>::const_iterator it = clients.begin();
    for (; it != clients.end(); ++it)
    {
        Client *client = it->second;
        if (channel->isMember(client))
        {
            std::cout << "Client FD: " << client->getFd()
                      << ", Nickname: " << client->getNickname()
                      << std::endl;
        }
    }

    std::cout << "------------------OPER--------------------" << std::endl;

    // Operators
    const std::set<Client*>& ops = channel->getOperators();
    std::set<Client*>::const_iterator op_it = ops.begin();
    for (; op_it != ops.end(); ++op_it)
    {
        Client *op = *op_it;
        std::cout << "Client FD: " << op->getFd()
                  << ", Nickname: " << op->getNickname()
                  << std::endl;
    }

    std::cout << "-----------------INVITED--------------------" << std::endl;

    const std::set<Client*>& inv = channel->getInvitedClients();
    std::set<Client*>::const_iterator inv_it = inv.begin();
    for (; inv_it != inv.end(); ++inv_it)
    {
        Client *cl = *inv_it;
        std::cout << "Client FD: " << cl->getFd()
                  << ", Nickname: " << cl->getNickname()
                  << std::endl;
    }

    std::cout << "----------------------------------------" << std::endl;
}
