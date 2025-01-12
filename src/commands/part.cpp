/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 23:14:34 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/28 23:25:06 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    partCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients,
    std::map<std::string, std::set<int>>& channels)
{
    if (command[0] == "PART") {
        if (command.size() <= 1) {
            sendMessage(client_fd, "ERROR :PART command requires a channel name", clients);
            return;
        }
    
        std::string channel_name = command[1];
        std::string part_message = "Left the channel";
    
        // Construct the parting message if provided
        if (command.size() > 2) {
            std::ostringstream part_message_stream;
            part_message_stream << command[2];
            for (size_t i = 3; i < command.size(); ++i) {
                part_message_stream << " " << command[i];
            }
            part_message = part_message_stream.str();
        }
    
        // Check if the channel exists
        auto channel_it = channels.find(channel_name);
        if (channel_it == channels.end()) {
            sendMessage(client_fd, "ERROR :No such channel " + channel_name, clients);
            std::cout << "\033[33m[INFO] : Client " << client_fd << " tried to leave a non-existent channel: " << channel_name << "\033[0m" << std::endl;
            return;
        }
    
        // Check if the client is a member of the channel
        if (!channel_it->second.count(client_fd)) {
            sendMessage(client_fd, "ERROR :You are not in channel " + channel_name, clients);
            std::cout << "\033[33m[INFO] : Client " << client_fd << " tried to leave channel " << channel_name
                      << " but is not a member.\033[0m" << std::endl;
            return;
        }
    
        // Notify other members in the channel
        for (int member_fd : channel_it->second) {
            if (member_fd != client_fd) {
                sendMessage(member_fd, ":" + clients[client_fd].nickname + " PART " + channel_name + " :" + part_message, clients);
            }
        }
    
        // Remove the client from the channel
        channel_it->second.erase(client_fd);
        sendMessage(client_fd, "NOTICE AUTH :You left the channel " + channel_name, clients);
        std::cout << "\033[35m[INFO] : Client " << client_fd << " left channel: " << channel_name << "\033[0m" << std::endl;
    
        // Remove the channel if it is empty
        if (channel_it->second.empty()) {
            std::cout << "\033[35m[INFO] : Channel " << channel_name << " is now empty and removed.\033[0m" << std::endl;
            channels.erase(channel_it);
        }
    }
}

// /alias add partclose /part $1; /if (irc.server_buffers[$server][$1]) /close
