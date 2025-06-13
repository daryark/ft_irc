//
// Created by mperetia on 08.06.25.
//

#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include <vector>
#include <string>
#include "Command.hpp"

// server
/*void Server::handleInput(Client* client, const std::string& input) {
    Command command = CommandFactory::parse(input, this);
    command.executeCommand(client);
}*/

class Server;
// class Command;
class CommandFactory
{
public:
    static Command parse(Server *server, const std::string &input)
    {
        std::istringstream iss(input);
        std::string command;
        iss >> command;

        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg){
          if(arg[0] == ':'){
              std::string rest;
              std::getline(iss, rest);
              arg = arg.substr(1) + rest;
              args.push_back(arg);
              break;
          }
        }
            args.push_back(arg);

        return Command(server, command, args);
    }
};

#endif // COMMANDFACTORY_H
