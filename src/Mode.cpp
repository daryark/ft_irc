#include "../incl/Command.hpp"

void Command::executeMode(Client *client)
{
    if (!checkPreconditions(client, 1))
        return ;
    const std::string &channelName = _args[0];
    Channel* channel = _server->getChannelByName(channelName);

    if(!channel)
        return client->queueMsg("403 " + channelName + " :No such channel");

    //only made #channel
    if(_args.size() == 1) {
        std::string modes = "+";
        if(channel->hasPassword())
            modes += "k";
        if(channel->isInviteOnly())
            modes += "i";
        if(channel->isTopicSetByOperator())
            modes += "t";
        if(channel->hasLimit())
            modes += "l";
        return client->queueMsg("324 " + channelName + " " + modes);
    }

    // check is operator
    if(!channel->isOperator(client))
        return client->queueMsg("482 " + channelName + " :You're not channel operator");

    bool adding = true;

    size_t argIndex = 2;
    std::string modeChanges;
    std::string modes;

    for (size_t i = 0; i < _args.size(); i++)
    {
        const std::string &modeStr = _args[i];
        for (size_t j = 0; j < modeStr.size(); j++)
        {
            char mode = modeStr[j];
            if (mode == '+')
                adding = true;
            else if (mode == '-')
                adding = false;
            
            switch (mode)
            {
                case 'i':
                    channel->setInviteOnly(adding);
                    modeChanges += (adding ? "+i " : "-i ");
                    break;
                case 't':
                    channel->setTopicSetByOperator(adding);
                    modeChanges += (adding ? "+t " : "-t ");
                    break;
                case 'k':
                    if (adding)
                    {
                        if (argIndex >= _args.size())
                            return client->queueMsg("461 MODE :Not enough parameters for +k");
                        const std::string &newPass = _args[argIndex++];
                        channel->setPassword(newPass);
                        modeChanges += "+k ";
                        modes += " " + newPass;
                    }
                    else
                    {
                        channel->setPassword("");
                        modeChanges += "-k ";
                    }
                    break;
                case 'o':
                    if(argIndex >= _args.size())
                        return client->queueMsg("461 MODE :Not enough parameters for o");
                    {
                        std::string nick = _args[argIndex++];
                        Client *target = _server->getClientByNickname(nick);
                        if (!target)
                        {
                            client->queueMsg("401 " + nick + " :No such nick");
                            continue;
                        }
                        if (!channel->isMember(target))
                        {
                            client->queueMsg("441 " + nick + " " + channelName + " :They aren't on that channel");
                            continue;
                        }
                        if (adding)
                            channel->addOperator(target);
                        else
                            channel->removeOperator(target);
                        modeChanges += (adding ? "+o" : "-o");
                        modes += " " + nick;
                    }
                    break;
                case 'l':
                    if (adding)
                    {
                        if (argIndex >= _args.size())
                            return client->queueMsg("461 MODE :Not enough parameters for +l");
                        int maxClients = std::stoi(_args[argIndex++]);
                        channel->setMaxClients(maxClients);
                        modeChanges += "+l ";
                        modes += " " + std::to_string(maxClients);
                    }
                    else
                    {
                        channel->setMaxClients(0);
                        modeChanges += "-l ";
                    }
                    break;
           default:
           client->queueMsg("472 " + std::string(1, mode) + " :is unknown mode char to me");
           break;

        }
    }

    if(!modeChanges.empty())
    {
        modeChanges.pop_back(); // remove trailing space
        channel->globalMessage(client, "MODE " + channelName + " " + modeChanges + modes);
    }
}
}
