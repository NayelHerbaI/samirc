/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 23:06:05 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 09:55:11 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void joinCommand(
    const std::vector<std::string>& command,
    const std::string& server_password,
    std::map<int, bool>& isAuthenticated,
    std::map<int, Client>& clients,
    int client_fd,
    std::map<std::string, std::set<int>>& channels,
    std::map<std::string, std::set<int>>& inviteList,
    std::map<std::string, std::set<char>>& channelModes,
    std::map<std::string, std::string>& channelKeys,        // for +k
    std::map<std::string, int>& channelUserLimits           // for +l
    // std::map<std::string, std::set<int>>& channelOperators // optional, if you want
)
{
    // Basic usage: JOIN #channel [key]
    if (command.empty() || command[0] != "JOIN")
        return;

    if (command.size() < 2) {
        sendMessage(client_fd, "ERROR :No channel name provided", clients);
        return;
    }

    std::string channel_name = command[1];

    // Validate that the channel name starts with '#'
    if (channel_name.empty() || channel_name[0] != '#') {
        sendMessage(client_fd, "ERROR :Invalid channel name. Channel names must start with #", clients);
        return;
    }

    // Check if channel exists
    auto it = channels.find(channel_name);
    bool is_new_channel = (it == channels.end());

    // Create channel if it doesn't exist
    if (is_new_channel) {
        channels[channel_name] = {};
        // Initialize channelModes if needed
        if (channelModes.find(channel_name) == channelModes.end())
            channelModes[channel_name] = {};

        std::cout << "[INFO] : Created new channel: " << channel_name << std::endl;

        // Optional: auto-operator. If you want the first user to become the channel operator, do:
        // channelOperators[channel_name].insert(client_fd);
        // std::cout << "[INFO] : Client " << client_fd << " is now the channel operator of " << channel_name << std::endl;
    }

    // --- Check +k (channel key) ---
    if (channelModes[channel_name].count('k') > 0) {
        // The channel is +k => require the user to provide a key in command[2]
        if (command.size() < 3) {
            sendMessage(client_fd, "ERROR :Channel " + channel_name + " is +k. A key is required.", clients);
            return;
        }
        std::string providedKey = command[2];  // e.g. JOIN #channel secretKey
        // Compare to stored key
        if (providedKey != channelKeys[channel_name]) {
            sendMessage(client_fd, "ERROR :Cannot join " + channel_name + ", invalid key.", clients);
            return;
        }
    }

    // --- Check +l (user limit) ---
    if (channelModes[channel_name].count('l') > 0) {
        int limit = channelUserLimits[channel_name];
        int currentSize = static_cast<int>(channels[channel_name].size());
        if (currentSize >= limit) {
            // Channel is full
            sendMessage(client_fd, "ERROR :Channel " + channel_name + " is full (limit " 
                                   + std::to_string(limit) + ")", clients);
            return;
        }
    }

    // --- Check +i (invite-only) ---
    std::cout << "[DEBUG] In joinCommand: Checking if " 
              << channel_name << " is invite-only..." << std::endl;
    if (channelModes[channel_name].count('i') > 0) {
        std::cout << "  --> " << channel_name << " is invite-only (+i)" << std::endl;
        if (inviteList[channel_name].count(client_fd) == 0) {
            std::cout << "  --> Client " << client_fd 
                      << " is NOT invited; rejecting join request." << std::endl;
            sendMessage(client_fd, "ERROR :You must be invited to join " + channel_name, clients);
            std::cout << "\033[31m[ERROR] : Client " << client_fd 
                      << " denied access to invite-only channel " 
                      << channel_name << " due to lack of invitation.\033[0m" << std::endl;
            return;
        }
        // If invited => remove from inviteList
        inviteList[channel_name].erase(client_fd);
        std::cout << "\033[35m[INFO] : Client " << client_fd 
                  << " removed from invite list for channel " 
                  << channel_name << ".\033[0m" << std::endl;
    } else {
        std::cout << "  --> " << channel_name << " is NOT invite-only" << std::endl;
    }

    // --- Now we can let them join ---
    channels[channel_name].insert(client_fd);
    clients[client_fd].joinedChannels.insert(channel_name);

    // Notify client of success
    sendMessage(client_fd, ":" + clients[client_fd].nickname + " JOIN " + channel_name, clients);
    std::cout << "\033[35m[INFO] : Client " << client_fd << " joined channel: " 
              << channel_name << "\033[0m" << std::endl;

    // Notify other members
    for (int member_fd : channels[channel_name]) {
        if (member_fd != client_fd)
            sendMessage(member_fd, ":" + clients[client_fd].nickname + " JOIN " + channel_name, clients);
    }
}
