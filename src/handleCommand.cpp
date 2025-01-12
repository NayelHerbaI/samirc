/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleCommand.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 20:32:03 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/31 18:11:27 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/irc.hpp"

void handleCommand(
    const std::vector<std::string>& command,
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

    )
{
    passCommand(command, server_password, isAuthenticated, clients, client_fd);
    userCommand(command, server_password, isAuthenticated, clients, client_fd);
    joinCommand(command, server_password, isAuthenticated, clients, client_fd, channels, inviteList, channelModes, channelKeys, channelUserLimits);
    pingCommand(command, client_fd, clients);
    msgCommand(command, client_fd, clients, channels);
    partCommand(command, client_fd, clients, channels);
    operCommand(command, client_fd, clients);
    whoisCommand(command, client_fd, clients);
    kickCommand(command, client_fd, clients, channels);
    modeCommand(command, client_fd, clients, channels, channelModes, channelKeys, channelOperators, channelUserLimits);
    inviteCommand(command, client_fd, clients, channels, inviteList);
    topicCommand(command, client_fd, clients, channels, channelTopics, channelModes, channelOperators);
    // debugServerState(channels, channelModes, inviteList, clients);
}