#include "../incl/Command.hpp"

void Command::executeMode(Client *client)
{
	if (!checkPreconditions(client, 1))
		return;

	const std::string &channelName = _args[0];
	Channel *channel = _server->getChannelByName(channelName);

	if (!channel)
		return client->queueMsg(ERR_NOSUCHCHANNEL(channelName));

	// only made #channel
	if (_args.size() == 1)
	{
		std::string modes = "+";
		if (channel->hasPassword())
			modes += "k";
		if (channel->isInviteOnly())
			modes += "i";
		if (channel->isTopicSetByOperator())
			modes += "t";
		if (channel->hasLimit())
			modes += "l";
		return client->queueMsg(RPL_CHANNELMODEIS(client->getNickname(), channelName, modes));
	}

	// check is operator
	if (!channel->isOperator(client))
		return client->queueMsg(ERR_CHANOPRIVSNEEDED(channelName));

	bool adding = true;

	size_t argIndex = 2;

	std::string modeChanges;
	std::string modes = _args[1];

	for (size_t i = 0; i < modes.size(); i++)
	{
		char mode = modes[i];
		if (mode == '+')
		{
			adding = true;
			continue;
		}
		else if (mode == '-')
		{
			adding = false;
			continue;
		}

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
					return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +k"));
				const std::string &newPass = _args[argIndex++];
				channel->setPassword(newPass);
				modeChanges += "+k " + newPass;
				// modes += " " + newPass;
			}
			else
			{
				channel->deletePassword();
				modeChanges += "-k ";
			}
			break;
		case 'o':
			if (argIndex >= _args.size())
				return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +o"));
			{
				std::string nick = _args[argIndex++];
				Client *target = _server->getClientByNickname(nick);
				if (!target)
				{
					client->queueMsg(ERR_NOSUCHNICK(nick));
					continue;
				}
				if (!channel->isMember(target))
				{
					client->queueMsg(ERR_USERNOTINCHANNEL(nick, channelName));
					continue;
				}
				if (adding)
				{
					channel->addOperator(target);
				}
				else
					channel->removeOperator(target);
				modeChanges += (adding ? "+o " : "-o ") + nick;
				// modes += " " + nick;

				channel->globalMessage(target, RPL_NAMREPLY(target->getNickname(), channel->getName(), formChannelMembersList(channel)), true);
				channel->globalMessage(target, RPL_ENDOFNAMES(client->getNickname(), channel->getName()), true);
				// client->queueMsg(RPL_ENDOFNAMES(client->getNickname(), channel->getName()));
			}
			break;
		case 'l':
			if (adding)
			{
				if (argIndex >= _args.size())
					return client->queueMsg(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE +l"));
				int maxClients = std::atoi(_args[argIndex++].c_str()); //c++11 int maxClients = std::atoi(_args[argIndex++].c_str());
				channel->setMaxClients(maxClients);
				std::ostringstream oss;
				oss << maxClients;
				modeChanges += "+l " + oss.str();

				// modeChanges += "+l " + std::to_string(maxClients);//c++11
				// modes += " " + std::to_string(maxClients);
			}
			else
			{
				channel->deleteLimits();
				modeChanges += "-l ";
			}
			break;
		default:
			client->queueMsg(ERR_UNKNOWNMODE(client->getNickname(), mode));
			break;
		}
	}
	if (!modeChanges.empty())
	{
		// modeChanges.pop_back(); // remove trailing space
		// channel->globalMessage(client, "MODE " + channelName + " " + modeChanges + modes + "\r\n", true);
		channel->globalMessage(client,
							   MSG(client->getNickname(), client->getUsername(), client->getHostname(),
								   "MODE ", channelName, modeChanges),
							   true);
	}
}
