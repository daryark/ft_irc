#include "../incl/Server.hpp"

void Server::pushPollfd(int fd, short event, short revent)
{
    pollfd new_pollfd;
    new_pollfd.fd = fd;
    new_pollfd.events = event;
    new_pollfd.revents = revent;
    _pollfds.push_back(new_pollfd);
}

void Server::fillSockaddrIn(struct sockaddr_in& addr, short int in_family, unsigned short int in_port, uint32_t s_addr)
{
    std::memset(&addr, 0, sizeof(addr));//*4
    addr.sin_family = in_family;
    addr.sin_port = htons(in_port);//*5
    addr.sin_addr.s_addr = htonl(s_addr);// inet_pton(AF_INET, "0.0.0.0", &_addr.sin_addr);
}

void Server::markPfdForPollout(int fd)
{
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
    {
        if (it->fd == fd) //#!vector std::find_if
        {
            it->events |= POLLOUT;
            break;
        }
    }
}

void Server::setSocketNonBlock(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) //*3
        throw std::runtime_error("fcntl");
}

void Server::checkClientsTimeouts()
{
    time_t now = time(NULL);

    for (std::map<int, Client*>::iterator it = _clients.begin();
        it != _clients.end(); it++)
    {
        Client* client = it->second;
        bool is_ping_sent = client->isPingSent();
        time_t last_activity = client->getLastActivityTime();

        if (!is_ping_sent && (now - last_activity >= PING_INTERVAL))
        {
            // std::string msg = "PING :" + static_cast<std::string>(SERVER_NAME) + "\r\n";
            // send(client->getFd(), msg.c_str(), msg.size(), 0);
            client->queueMsg("PING :" + static_cast<std::string>(SERVER_NAME) + "\r\n");
            client->setPingSent(true);
            client->setLastActivityTime(now);
        }
        else if (is_ping_sent && last_activity >= PONG_TIMEOUT)
            disconnectClient(client->getFd());
    }
}

void    Server::fancyPrint(const std::string& opt)
{
    std::cout << std::endl << B_BLUE << opt;
    if (opt == PR_RUN)
            std::cout << " ༼ つ " << B_RED
        << "♥" << B_BLUE << "_" << B_RED << "♥" << B_BLUE << " ༽つ "  << B_BLUE 
        << _port << ",   password '" << _password << "'";
    else if (opt == PR_CLOSE)
        std::cout << B_RED << " ⊂༼" << B_BLUE
        << "´סּ" << B_RED << "︵" << B_BLUE "סּ`" << B_RED << "⊂ ༽";
    std::cout << RE << std::endl;
}