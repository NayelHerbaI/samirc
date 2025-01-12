/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   whois.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 19:02:17 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/29 19:04:34 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    whoisCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients
)
{
    if (command[0] == "WHOIS") {
        if (command.size() > 1) {
            std::string target_nickname = command[1];
            bool found = false;
    
            for (const auto& [fd, client] : clients) {
                if (client.nickname == target_nickname) {
                    found = true;
                    std::string role = client.isOperator ? "Operator" : "Regular User";
                    sendMessage(client_fd, "NOTICE AUTH :User " + target_nickname + " is a " + role, clients);
                    std::cout << "[INFO] : Client " << client_fd << " checked status of " << target_nickname
                              << ": " << role << std::endl;
                    break;
                }
            }
    
            if (!found) {
                sendMessage(client_fd, "ERROR :User " + target_nickname + " not found.", clients);
            }
        } else {
            sendMessage(client_fd, "ERROR :WHOIS command requires a target nickname.", clients);
        }
    }
}