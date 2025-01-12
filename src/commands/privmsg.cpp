/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 23:11:30 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/28 23:13:28 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    msgCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients,
    std::map<std::string, std::set<int>>& channels)
{
    if (command[0] == "PRIVMSG") {
        if (command.size() > 2) {
            std::string target = command[1];
            std::string message;
    
            // Combine all parts of the message into a single string
            for (size_t j = 2; j < command.size(); ++j) {
                if (j > 2) message += " ";
                message += command[j];
            }
    
            if (target[0] == '#') {
                // Check if the client is a member of the channel
                if (channels.find(target) != channels.end() && channels[target].count(client_fd)) {
                    // Broadcast to channel members
                    for (int member_fd : channels[target]) {
                        if (member_fd != client_fd) {
                            sendMessage(member_fd, ":" + clients[client_fd].nickname + " PRIVMSG " + target + " " + message, clients);
                        }
                    }
                    std::cout << "\033[35m[INFO] : Message sent to channel " << target << ": " << message << std::endl;
                } else {
                    // Send an error and prevent further processing
                    sendMessage(client_fd, "ERROR :You are not in channel " + target, clients);
                    std::cout << "\033[35m[INFO] : Client " << client_fd << " attempted to send message to channel " << target << " but is not a member." << std::endl;
                    return; // Ensure no further processing occurs
                }
            } else {
                // Send private message to a specific user
                bool user_found = false;
                for (const auto& [fd, client] : clients) {
                    if (client.nickname == target) {
                        sendMessage(fd, ":" + clients[client_fd].nickname + " PRIVMSG " + target + " " + message, clients);
                        user_found = true;
                        break;
                    }
                }
                if (!user_found) {
                    sendMessage(client_fd, "ERROR :User " + target + " not found", clients);
                }
            }
        } else {
            sendMessage(client_fd, "ERROR :Invalid PRIVMSG syntax", clients);
        }
    }
}