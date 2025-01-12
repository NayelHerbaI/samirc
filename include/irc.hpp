/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 16:07:19 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/31 18:11:04 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <stdio.h>
#include <sys/types.h>
#include <sstream>
#include <map>
#include <unistd.h>
#include <set>

#define PORT 6667
#define MAX_CLIENTS 100
#define BUFFER_SIZE 512

extern bool server_running;

struct Client
{
    std::string nickname;
    std::string username;
    std::string proposedNickname; // Temporary nickname until confirmed
    std::string proposedUsername; // Temporary username until confirmed
    bool authenticated;
    bool isOperator; // True for operators, false for regular users
    std::set<std::string> joinedChannels;    // Channels the client has joined
};

int                                 setupServerSocket(int port);
void                                handleClients(int server_fd, const std::string& server_password);
std::vector<std::string>            parseCommand(const std::string &input);
void                                setupSignalHandler();
void                                setNonBlockingMode(int fd);
bool                                sendMessage(int client_fd, const std::string &message,
                                                std::map<int, Client>& clients);
void                                handleCommand(const std::vector<std::string>& command,
                                                int client_fd, 
                                                const std::string& server_password, 
                                                std::map<int, bool>& isAuthenticated, 
                                                std::map<int, Client>& clients,
                                                std::map<std::string, std::set<int>>& channels,
                                                std::map<std::string, std::set<int>>& inviteList,
                                                std::map<std::string, std::set<char>>& channelModes,
                                                std::map<std::string, std::string>& channelTopics,
                                                std::map<std::string, int>& channelUserLimits,       // NEW
                                                std::map<std::string, std::set<int>>& channelOperators,  // NEW
                                                std::map<std::string, std::string>& channelKeys          // NEW
);
void                                passCommand(const std::vector<std::string>& command,
                                                const std::string& server_password,
                                                std::map<int, bool>& isAuthenticated,
                                                std::map<int, Client>& clients,
                                                int client_fd
                                                );
void                                userCommand(const std::vector<std::string>& command,
                                                const std::string& server_password,
                                                std::map<int, bool>& isAuthenticated,
                                                std::map<int, Client>& clients,
                                                int client_fd
                                                );
void                                joinCommand(const std::vector<std::string>& command,
                                                const std::string& server_password,
                                                std::map<int, bool>& isAuthenticated,
                                                std::map<int, Client>& clients,
                                                int client_fd,
                                                std::map<std::string, std::set<int>>& channels,
                                                std::map<std::string, std::set<int>>& inviteList,
                                                std::map<std::string, std::set<char>>& channelModes,
                                                std::map<std::string, std::string>& channelKeys,        // for +k
                                                std::map<std::string, int>& channelUserLimits           // for +l
);
void                                pingCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients);
void                                msgCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients,
                                                std::map<std::string, std::set<int>>& channels);
void                                partCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients,
                                                std::map<std::string, std::set<int>>& channels);
void                                operCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients);
void                                whoisCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients);
void                                kickCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients,
                                                std::map<std::string, std::set<int>>& channels);
void                                inviteCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients,
                                                std::map<std::string, std::set<int>>& channels,
                                                std::map<std::string, std::set<int>>& inviteList);
void                                modeCommand(const std::vector<std::string>& command,
                                                int client_fd,
                                                std::map<int, Client>& clients,
                                                std::map<std::string, std::set<int>>& channels,
                                                std::map<std::string, std::set<char>>& channelModes,
                                                std::map<std::string, std::string>& channelKeys,         // NEW
                                                std::map<std::string, std::set<int>>& channelOperators,  // NEW
                                                std::map<std::string, int>& channelUserLimits            // NEW
);
void                                debugServerState(std::map<std::string, std::set<int>>& channels,
                                                std::map<std::string, std::set<char>>& channelModes,
                                                const std::map<std::string, std::set<int>>& inviteList,
                                                const std::map<int, Client>& clients);
void                                topicCommand(const std::vector<std::string> &command,
                                                int client_fd,
                                                std::map<int, Client> &clients,
                                                std::map<std::string, std::set<int>> &channels,
                                                std::map<std::string, std::string> &channelTopics,
                                                std::map<std::string, std::set<char>>& channelModes, // We'll need to check +t here
                                                std::map<std::string, std::set<int>>& channelOperators
);
#endif