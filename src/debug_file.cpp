#include "../incl/Client.hpp"
#include "../incl/Server.hpp"
#include "../incl/Command.hpp"


void Command::PrintAllClients( Server& server) {
    const std::map<int, Client*>& clients = server.getClients();
    std::cout << "----- Clients List -----" << std::endl;
    for ( auto& pair : clients) {
         Client* client = pair.second;
        std::cout << "FD: " << client->getFd()
                  << ", Nickname: " << client->getNickname()
                  << ", Authenticated: " << (client->isAuthenticated() ? "Yes" : "No")
                  << ", Registered: " << (client->isRegistered() ? "Yes" : "No")
                  << std::endl;
    }
    std::cout << "------------------------" << std::endl;
}

void Command::PrintAllChannels( Server& server) {

    const std::map<std::string, Channel*>& channels = server.getChannel();
    std::cout << "Size of channels map: "<<channels.size()<<std::endl;
    std::cout << "----- Channels List -----" << std::endl;
    for ( auto& pair : channels) {
         Channel* channel = pair.second;
        std::cout << "Channel Name: " << channel->getName()
                  << ", Topic: " << channel->getTopic()
                  << ", Number of Clients: " << channel->getSize()
                  << std::endl;
    }
    std::cout << "-------------------------" << std::endl;
}

void Command::PrintMembersInChannel( Server& server, const std::string& channelName) {
    Channel* channel = server.getChannelByName(channelName);
    if (!channel) {
        std::cout << "Channel " << channelName << " does not exist." << std::endl;
        return;
    }

    for (const auto& pair : server.getClients()) {
        Client* client = pair.second;
        if (channel->isMember(client)) {
            std::cout << "Client FD: " << client->getFd()
                      << ", Nickname: " << client->getNickname()
                      << std::endl;
        }
    }
    std::cout << "----------------------------------------" << std::endl;
}