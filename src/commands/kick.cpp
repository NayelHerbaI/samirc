/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 19:12:53 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 00:30:47 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    kickCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients,
    std::map<std::string, std::set<int>>& channels
)

{
    if (command[0] == "KICK") {
        if (!clients[client_fd].isOperator) {
            sendMessage(client_fd, "ERROR :You must be an operator to use this command.", clients);
            return;
        }
    
        if (command.size() < 3) {
            sendMessage(client_fd, "ERROR :KICK command requires a channel and a target nickname.", clients);
            return;
        }
    
        std::string channel_name = command[1];
        std::string target_nickname = command[2];
        std::string reason = (command.size() > 3) ? command[3] : "No reason provided";
    
        // Check if the channel exists
        auto channel_it = channels.find(channel_name);
        if (channel_it == channels.end()) {
            sendMessage(client_fd, "ERROR :Channel " + channel_name + " does not exist.", clients);
            return;
        }
    
        // Find the target user in the channel
        int target_fd = -1;
        for (int member_fd : channel_it->second) {
            if (clients[member_fd].nickname == target_nickname) {
                target_fd = member_fd;
                break;
            }
        }
    
        if (target_fd == -1) {
            sendMessage(client_fd, "ERROR :User " + target_nickname + " not found in channel " + channel_name + ".", clients);
            return;
        }
    
        // Notify the target user
        sendMessage(target_fd, ":ircserver KICK " + channel_name + " " + target_nickname + " :" + reason, clients);
        std::cout << "[INFO] : Client " << target_fd << " (" << target_nickname << ") was kicked from " 
                  << channel_name << " by " << clients[client_fd].nickname << std::endl;
    
        // Notify other members in the channel
        for (int member_fd : channel_it->second) {
            if (member_fd != target_fd) {
                sendMessage(member_fd, ":" + clients[client_fd].nickname + " KICK " + channel_name + " " + target_nickname + " :" + reason, clients);
            }
        }
    
        // Remove the target user from the channel
        channel_it->second.erase(target_fd);
    
        // Remove the channel from the client's joinedChannels set
        clients[target_fd].joinedChannels.erase(channel_name);
    
        // Notify the kicked client
        sendMessage(target_fd, "NOTICE AUTH :You have been kicked from " + channel_name, clients);
    
        // Check if the channel is now empty
        if (channel_it->second.empty()) {
            channels.erase(channel_it);
            std::cout << "[INFO] : Channel " << channel_name << " is now empty and has been removed." << std::endl;
        }
    }
}