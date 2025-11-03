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
            std::cout << GREEN << "REST: " << rest << RE << std::endl;
            if (!rest.empty())
            {
              if (arg[1])
                args.push_back(arg.substr(1) + rest);
              else
                args.push_back(rest);
            }
            else if (arg[1])
              args.push_back(arg.substr(1));
            break ;
          }
          if (!arg.empty())
            args.push_back(arg);
        }
        std::cout << BG_RED << args.size() << " args.size" << RE << std::endl;//########################
        for (size_t i = 0; i < args.size(); i++)  //########################
            std:: cout << BG_MAGENTA << args[i] << " args[" << i << "]" << RE << std::endl;//########################
        return Command(server, command, args);
    }
};
