#include "../incl/Server.hpp"
#include "../incl/CommandFactory.hpp"

volatile sig_atomic_t g_runnning = 1;

void sigHandler(int)
{
    g_runnning = 0;
}

//* all the '*' explanation in extra/server.about file
Server::Server(int port, std::string password): _port(port), _password(password){}

Server::~Server()
{
    close(_head_socket);

    std::map<std::string, Channel*>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
    _channels.clear();

    std::map<int, Client*>::iterator it2;
    for (it2 = _clients.begin(); it2 != _clients.end(); ++it2)
        delete it2->second;
    _clients.clear();

    fancyPrint(PR_CLOSE);
}

void Server::init()
{
    _head_socket = socket(AF_INET, SOCK_STREAM, 0); //*1
    if (_head_socket == -1)
        throw std::runtime_error("socket");

    int opt = 1; // true;
    if (setsockopt(_head_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) //*2
        throw std::runtime_error("setsockopt");

    setSocketNonBlock(_head_socket);

    fillSockaddrIn(_addr, AF_INET, _port, INADDR_ANY);
    if (bind(_head_socket, (sockaddr *)&_addr, sizeof(_addr)) == -1)
        throw std::runtime_error("bind failed. Address is already in use");
    fancyPrint(PR_RUN);

    if (listen(_head_socket, SOMAXCONN) == -1)
        throw std::runtime_error("listen");
    fancyPrint(PR_LISTEN);
}

void Server::run()
{
    pushPollfd(_head_socket, POLLIN, 0); //#6

    signal(SIGINT, sigHandler);
    signal(SIGTSTP, sigHandler);

    while (g_runnning)
    {
        if (poll(_pollfds.data(), static_cast<int>(_pollfds.size()), 1000) == -1) //*7
            break ;

        checkClientsTimeouts();
        
        for (int i = 0; i < static_cast<int>(_pollfds.size()); )
        {
            if (actionOnFd(_pollfds[i]))
                i++;
        }
    }
}

bool Server::actionOnFd(pollfd& pollfd)
{
    if (pollfd.revents & (POLLHUP | POLLERR | POLLNVAL))
        return !cleanClient(pollfd.fd);
    else if (pollfd.revents & POLLIN) //*6.1
    {
        if (pollfd.fd == _head_socket)
            acceptClient();
        else
            return readMsg(pollfd.fd);
    }
    else if (pollfd.revents & POLLOUT)
        sendMsg(pollfd);
    return true;
}

void Server::acceptClient()
{
    sockaddr_in client;
    std::memset(&client, 0, sizeof(client));
    socklen_t   clSize = sizeof(client);
    int client_sock = accept(_head_socket, reinterpret_cast<sockaddr *>(&client), &clSize);
    if (client_sock == -1)
    {
        std::cerr << RED << PR_CL_NOT_CONNECT << RE << std::endl;
        return ;
    }
    pushPollfd(client_sock, POLLIN | POLLOUT, 0);
    setSocketNonBlock(client_sock);
    const std::string& ip = inet_ntoa(client.sin_addr);
    _clients.insert(std::pair<int, Client*>(client_sock, new Client(client_sock, ip, this)));

    std::cout << B_GREEN << PR_CL_CONNECT << client_sock << RE << std::endl;
    getClient(client_sock)->queueMsg(PR_IN_MSG);
}

bool Server::disconnectClient(int fd)
{
    std::map<int, Client*>::iterator erase_it = _clients.find(fd);
    if(erase_it == _clients.end())
        return false;

    Client* client = erase_it->second;
    
    std::map<std::string, Channel*>::iterator it_chan = _channels.begin();
    for (; it_chan != _channels.end(); ++it_chan)
    {
        Channel* chan = it_chan->second;
        if (chan->isMember(client))
            chan->removeClient(client);
    }


    delete erase_it->second;
    _clients.erase(erase_it); //map
    close(fd);
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
    {
        if (it->fd == fd) //#!vector std::find_if
        {
            std::cout << BG_RED << "Disconnected: " << fd << RE << std::endl;
            _pollfds.erase(it); //vector
            return true;
        }
    }
    return false;

}

bool Server::readMsg(int fd)
{
    char buf[MAX_MSG] = {0};
    int recv_bytes = recv(fd, buf, MAX_MSG, 0);
    if (recv_bytes <= 0)
    {
        std::cerr << (recv_bytes == 0 
            ? "Client disconnected on socket fd: " 
            : "Connection problem on fd: ") << fd <<std::endl;
        return !cleanClient(fd);
    }
    processInMsg(fd, buf, recv_bytes);
    return true;
}

void Server::processInMsg(int fd, char* buf, int len)
{
    Client* client = getClient(fd);
    if (!client)
        return ;
    
    client->updateActive();
    std::string& all_buf = client->getIncompleteMsg().append(buf, static_cast<size_t>(len)); //non-const Ref& of _incomplete_msg on Client
    
    size_t pos = all_buf.find("\n");
    while(pos != std::string::npos)
    {
        std::string line = all_buf.substr(0, pos);
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1); //erase 1 char inplace
        all_buf = all_buf.erase(0, pos + 1);
        if (!line.empty())
        {
            std::cout << BG_I_YELLOW << "LINE: " << line << RE << std::endl;//############
            Command command = CommandFactory::parse(this,line);
            command.executeCommand(client);
        }
        pos = getClient(fd) ? all_buf.find("\n") : std::string::npos;
    }
}

void Server::sendMsg(pollfd& pollfd)
{
    Client* client = getClient(pollfd.fd);
    std::deque<std::string>& msg_queue = client->getMsgQueue();
    while(!msg_queue.empty())
    {
        ssize_t n = send(pollfd.fd, msg_queue.front().c_str(), msg_queue.front().size(), MSG_NOSIGNAL);
        if (n <= 0)
            break; // socket full, wait for next POLLOUT
        msg_queue.pop_front();
    }
    pollfd.events &= ~POLLOUT; // stop monitoring POLLOUT until new msg
}


//*getters
const std::string& Server::getPassword() const { return _password; }
const std::map<int, Client*>& Server::getClients() const{ return _clients; }
const std::map<std::string, Channel*>& Server::getChannel() const {return _channels;}

Client* Server::getClient(int fd)   const
{
    std::map<int, Client*>::const_iterator it = _clients.find(fd);
    if (it != _clients.end())
        return it->second;
    return NULL;
}

Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Client* Server::getClientByNickname(const std::string& nickname) {
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == nickname) { //#!vector std::find_if
            return it->second;
        }
    }
    return NULL;
    // return std::find(_clients.begin(), _clients.end(), nickname);
}

Channel* Server::createChannel(const std::string& channel_name, const std::string& channel_password) {
    Channel *channel = new Channel(channel_name, channel_password);
    _channels[channel_name] = channel;
    return channel;
}

void Server::deleteChannel(const std::string& channel_name)
{
    Channel *channel = getChannelByName(channel_name);
    if (channel)
    {
        _channels.erase(channel_name);
        delete channel;
        std::cout << B_RED << "channel: " << channel_name << " deleted" << RE << std::endl; //##########
    }
}

// void Server::delete
