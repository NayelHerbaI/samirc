/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 00:39:07 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 03:09:24 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/irc.hpp"

void debugServerState(
    std::map<std::string, std::set<int>>& channels,
    std::map<std::string, std::set<char>>& channelModes,
    const std::map<std::string, std::set<int>>& inviteList,
    const std::map<int, Client>& clients
) {
    // Start printing in white
    std::cout << "\033[37m";  // Set foreground color to white

    std::cout << "\n===================== SERVER STATE DEBUG =====================" << std::endl;

    // // Manually set invite-only mode for testing
    // std::string test_channel = "#testchannel";
    // channelModes[test_channel].insert('i');
    // std::cout << "[DEBUG] Manually set invite-only mode (+i) for " << test_channel << std::endl;

    // Channels
    std::cout << "[DEBUG] Channels and Members:" << std::endl;
    for (const auto& [channel, members] : channels) {
        std::cout << "  " << channel << " (Members: ";
        for (int fd : members) std::cout << fd << " ";
        std::cout << ")" << std::endl;
    }

    // Channel Modes
    std::cout << "[DEBUG] Channel Modes:" << std::endl;
    for (const auto& [channel, modes] : channelModes) {
        std::cout << "  " << channel << " (Modes: ";
        if (modes.empty()) {
            std::cout << "None";
        } else {
            for (char mode : modes) std::cout << mode << " ";
        }
        std::cout << ")" << std::endl;
    }

    // Invite Lists
    std::cout << "[DEBUG] Invite Lists:" << std::endl;
    for (const auto& [channel, invites] : inviteList) {
        std::cout << "  " << channel << " (Invited: ";
        for (int fd : invites) std::cout << fd << " ";
        std::cout << ")" << std::endl;
    }

    // Clients
    std::cout << "[DEBUG] Clients:" << std::endl;
    for (const auto& [fd, client] : clients) {
        std::cout << "  FD: " << fd << " (Nickname: " << client.nickname
                  << ", Username: " << client.username
                  << ", Operator: " << (client.isOperator ? "Yes" : "No")
                  << ", Channels: ";
        for (const auto& channel : client.joinedChannels) std::cout << channel << " ";
        std::cout << ")" << std::endl;
    }

    std::cout << "=============================================================\n" << std::endl;

    // Reset color back to default
    std::cout << "\033[0m";
}