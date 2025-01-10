#pragma once

#include <iostream>
#include <map>

class Client;

class Channel
{
private:
	std::string _name;
	std::string _topic;
	
	// std::set<int> _clients;
	// std::set<int> _operators;

	std::map<int, Client*> members;     
    std::map<int, Client*> operators;
	
	bool _is_private;
	std::string _password;
	
	int _max_clients;
	// bool _is_invite_only;

public:
	Channel();
	Channel(std::string name);

	Channel(const Channel &obj);
	Channel &operator=(const Channel &obj);
	~Channel();

	// metchods
	void setTopic(const std::string& topic);
    const std::string& getTopic() const;

    void addClient(int client_fd);
    void removeClient(int client_fd);
    bool hasClient(int client_fd) const;

    void addOperator(int client_fd);
    void removeOperator(int client_fd);
    bool isOperator(int client_fd) const;

    // void broadcast(const std::string& message, int exclude_fd = -1) const;
};