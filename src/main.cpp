#include "../incl/Server.hpp"
#include "../incl/Client.hpp"
#include "../incl/Command.hpp"

void    ft_irc(Server& serv)
{
    try {
        serv.init();
        serv.run();
    }
    catch (std::exception& e) {
        std::cout << U_RED << e.what() << RE << std::endl;
    }
    return ;
}

//#ex:  ./ircserv <port> <password>
int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << RED << "usage: ./ircserv <port> <password>" << RE << std::endl;
        return 1;
    }
    int port = atoi(argv[1]);
    if ( port < 1024 || port > 65535)
    {
        std::cerr << RED << "Port is not valid. Must be in range: 1024 - 65535" << RE << std::endl;
        return 1;
    }

    Server  serv(port, argv[2]);
    ft_irc(serv);
    return 0;
}
