#include "../incl/Command.hpp"

std::map<std::string, Command::CommandHandler> Command::_commandMap;

Command::Command(Server* server,const std::string& command, const std::vector<std::string>& args)
    : _server(server), _command(command), _args(args)
{
	if(_commandMap.empty())
		initCommandMap();
}

void Command::initCommandMap() {
    _commandMap["USER"]    = &Command::executeUser;
    _commandMap["NICK"]    = &Command::executeNick;
    _commandMap["PASS"]    = &Command::executePass;
    _commandMap["JOIN"]    = &Command::executeJoin;
    _commandMap["PRIVMSG"] = &Command::executePrivmsg;
    _commandMap["KICK"]    = &Command::executeKick;
    _commandMap["INVITE"]  = &Command::executeInvite;
    _commandMap["TOPIC"]   = &Command::executeTopic;
    _commandMap["MODE"]    = &Command::executeMode;
}

Command::~Command()
{
}

void Command::executeCommand(Client* client, const std::vector<std::string>& args){
    std::map<std::string, CommandHandler>::iterator it = _commandMap.find(_command);
    if(it != _command.end()){
        CommandHandel handel = it->second;
        (this->*handel)(client);
    } else{
        std::cout << "Command not found" << std::endl;
    }

}

void Command::executePrivmsg(){
    // Реализация метода executePrivmsg
}

void Command::executeJoin() {
    // Реализация метода executeJoin
}

void Command::executePong() {
    // Реализация метода executePong
}

void Command::executeQuit() {
    // Реализация метода executeQuit
}

void Command::executePart() {
    // Реализация метода executePart
}

void Command::executeKick() {
    // Реализация метода executeKick
}

void Command::executeInvite() {
    // Реализация метода executeInvite
}

void Command::executeTopic() {
    // Реализация метода executeTopic
}

void Command::executeMode() {
    // Реализация метода executeMode
}

void Command::executeNick() {
    // Реализация метода executeNick
}

void Command::executePass() {
    // Реализация метода executePass
}
void Command::executeUser() {
    // Реализация метода executePass
}