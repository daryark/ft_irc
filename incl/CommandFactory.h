//
// Created by mperetia on 08.06.25.
//

#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H


//server
/*void Server::handleInput(Client* client, const std::string& input) {
    Command command = CommandFactory::parse(input, this);
    command.executeCommand(client);
}*/

class Server;
class Command;
class CommandFactory {
public:
    static Command parse(Server* server, const std::string& input) {
        std::istringstream iss(input);
        std::string command;
        iss >> command;

        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg)
            args.push_back(arg);

        return Command(server, command, args);
    }
};

#endif //COMMANDFACTORY_H
