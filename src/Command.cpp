#include "../incl/Command.hpp"

Command::Command()
{
}

Command::Command(const Command &obj)
{
	_command = obj._command;
	_params = obj._params;
	sender = obj.sender;
}

Command &Command::operator=(const Command &obj)
{
	_command = obj._command;
	_params = obj._params;
	sender = obj.sender;
	return *this;
}

Command::~Command()
{
}

enum CommandType Command::getTypeCommand()
{
	if (_command == "USER")
	{
		return USER;
	}
	else if (_command == "NICK")
	{
		return NICK;
	}
	else if (_command == "PASS")
	{
		return PASS;
	}
	else if (_command == "JOIN")
	{
		return JOIN;
	}
	else if (_command == "PRIVMSG")
	{
		return PRIVMSG;
	}
	else if (_command == "PING")
	{
		return PING;
	}
	else if (_command == "PONG")
	{
		return PONG;
	}
	else if (_command == "QUIT")
	{
		return QUIT;
	}
	else
	{
		return UNKNOWN;
	}
}

void Command::executeCommand()
{
	
	switch (getTypeCommand())
	{
	case 1:
		/* code */
		break;
	case 2:
		/* code */
		break;
	case 3:	
		/* code */
		break;
	case 4:
		/* code */
		break;
	case 5:
		/* code */
		break;
	
	default:
		break;
	}
	std::cout << "Command::executeCommand()" << std::endl;
}

void Command::parse(const std::string &input, Client *sender)
{
	

}