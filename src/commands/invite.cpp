/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 19:25:39 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 02:44:43 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    inviteCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients,
    std::map<std::string, std::set<int>>& channels,
    std::map<std::string, std::set<int>>& inviteList
)
{
    if (command[0] == "INVITE") {
        if (!clients[client_fd].isOperator) {
            sendMessage(client_fd, "ERROR :You must be an operator to use this command.", clients);
            return;
        }
    
        if (command.size() < 3) {
            sendMessage(client_fd, "ERROR :INVITE command requires a nickname and a channel.", clients);
            return;
        }
    
        std::string target_nickname = command[1];
        std::string channel_name = command[2];
    
        // Check if the channel exists
        auto channel_it = channels.find(channel_name);
        if (channel_it == channels.end()) {
            sendMessage(client_fd, "ERROR :Channel " + channel_name + " does not exist.", clients);
            return;
        }
    
        // Find the target user
        int target_fd = -1;
        for (const auto& [fd, client] : clients) {
            if (client.nickname == target_nickname) {
                target_fd = fd;
                break;
            }
        }
    
        if (target_fd == -1) {
            sendMessage(client_fd, "ERROR :User " + target_nickname + " not found.", clients);
            return;
        }
    
        // Check if the target user is already in the channel
        if (channel_it->second.count(target_fd)) {
            sendMessage(client_fd, "ERROR :User " + target_nickname + " is already in channel " + channel_name + ".", clients);
            return;
        }
    
        // Notify the invited user
        sendMessage(target_fd, ":" + clients[client_fd].nickname + " INVITE " + target_nickname + " " + channel_name, clients);
        std::cout << "[INFO] : Client " << target_fd << " (" << target_nickname << ") was invited to " 
                  << channel_name << " by " << clients[client_fd].nickname << std::endl;
    
        // Optionally track the invitation (for invite-only channels)
        inviteList[channel_name].insert(target_fd);
    
        // Notify the inviter
        sendMessage(client_fd, "NOTICE AUTH :Invitation sent to " + target_nickname + " for channel " + channel_name, clients);
    }

}