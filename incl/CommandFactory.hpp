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
    while (iss >> arg)
    {
      if (arg[0] == ':')
      {
        args.push_back(":");
        arg.erase(0, 1);

        std::string rest;
        std::getline(iss, rest);
        trim(rest);

        if (!arg.empty() && !rest.empty())
          arg += " ";
        arg += rest;
      }
      if (!arg.empty())
        args.push_back(arg);
    }
    std::cout << BG_RED << args.size() << " args.size" << RE << std::endl;// ########################
    for (size_t i = 0; i < args.size(); ++i) // ########################
      std::cout << BG_MAGENTA << args[i] << " args[" << i << "]" << RE << std::endl; //########################
    return Command(server, command, args);
  }
};

void trim(std::string &s)
{
    size_t start = s.find_first_not_of(" \t");
    size_t end   = s.find_last_not_of(" \t");

    if (start == std::string::npos)
    {
        s.clear();
        return;
    }

    s.erase(end + 1);
    s.erase(0, start);
}