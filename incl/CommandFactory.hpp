#pragma once

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
            args.push_back(":");
            std::string rest;
            std::getline(iss, rest);
            arg = arg.substr(1) + rest;
            args.push_back(arg);
            break;
          }
          args.push_back(arg);
        }
        std::cout << "args.size = " << args.size()<< "; ";//########################
        return Command(server, command, args);
    }
};

