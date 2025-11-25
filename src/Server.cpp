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

void    Server::init()
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
    bool client_disconnected = 0;
    while (g_runnning)
    {
        if (poll(_pollfds.data(), (int)_pollfds.size(), 1000) == -1) //*7
            break ;
        for (int i = 0; i < (int)_pollfds.size(); i++)
        {
            if (_pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
                client_disconnected = disconnectClient(_pollfds[i].fd);
            else if (_pollfds[i].revents & POLLIN) //*6.1
            {
                if (_pollfds[i].fd == _head_socket)
                   acceptClient();
                else
                    client_disconnected = readMsg(_pollfds[i].fd);
            }
            else if (_pollfds[i].revents & POLLOUT)
               sendMsg(_pollfds[i]);

            if (client_disconnected)
                --i;
        }
    }
}

void    Server::acceptClient()
{
    sockaddr_in client;
    std::memset(&client, 0, sizeof(client));
    socklen_t   clSize = sizeof(client);
    int client_sock = accept(_head_socket, (sockaddr*)&client, &clSize);
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

bool   Server::disconnectClient(int fd)
{
    std::map<int, Client*>::iterator client = _clients.find(fd);
    if(client == _clients.end())
        return false;
    delete client->second;
    _clients.erase(client); //map
    close(fd);
    
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
    {
        if (it->fd == fd)
        {
            std::cout << "disconnected" << "; ";
            _pollfds.erase(it); //vector
            return true;
        }
    }
    return false;
}

bool    Server::readMsg(int fd)
{
    char buf[MAX_MSG + 2] = {0};
    std::cout << B_YELLOW << "fd: " << fd << RE << "; ";//##########
    int recv_bytes = recv(fd, buf, MAX_MSG, 0);
    std::cout << "recived bytes: " << recv_bytes << "; ";//#########
    buf[recv_bytes] = '\r';//!!!
    buf[recv_bytes + 1] = '\n';//!!!
    if (recv_bytes <= 0)
    {
        std::cerr << (recv_bytes == 0 
            ? "Client disconnected on socket fd: " 
            : "Connection problem on fd: ") << fd <<std::endl;
            return disconnectClient(fd);
    }
    processInMsg(fd, buf, recv_bytes + 2);
    return false;
}

void    Server::processInMsg(int fd, char* buf, int len)
{
    Client* client = getClient(fd);
    if (!client)
    return ;
    
    std::string& all_buf = client->getIncompleteMsg().append(buf, static_cast<size_t>(len)); //non-const Ref& of _incomplete_msg
    
    size_t pos = 0;
    while((pos = all_buf.find("\r\n")) != std::string::npos)
    {
        std::string line = all_buf.substr(0, pos);
        all_buf.erase(0, pos + 2);
        if (line.empty())
        continue ;
        
        std::cout << BG_I_YELLOW << "LINE: " << line << RE << std::endl;//############
        Command command = CommandFactory::parse(this,line);
        command.executeCommand(client);
    }
}

void    Server::sendMsg(pollfd& pollfd)
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

//*------------------helpers--------------------
void    Server::markPfdForPollout(int fd)
{
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
    {
        if (it->fd == fd)
        {
            it->events |= POLLOUT;
            break;
        }
    }
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
        if (it->second->getNickname() == nickname) {
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
