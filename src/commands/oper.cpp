/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 18:32:08 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/29 18:35:28 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    operCommand(
        const std::vector<std::string>& command,
        int client_fd,
        std::map<int, Client>& clients
)
{
    if (command[0] == "OPER") {
        if (!clients[client_fd].isOperator) {
            sendMessage(client_fd, "ERROR :You must be an operator to use this command.", clients);
            return;
        }
    
        if (command.size() > 1) {
            std::string target_nickname = command[1];
            bool found = false;
    
            for (auto& [fd, client] : clients) {
                if (client.nickname == target_nickname) {
                    client.isOperator = true;
                    sendMessage(fd, "NOTICE AUTH :You have been promoted to operator.", clients);
                    sendMessage(client_fd, "NOTICE AUTH :Successfully promoted " + target_nickname + " to operator.", clients);
                    found = true;
                    break;
                }
            }
    
            if (!found)
                sendMessage(client_fd, "ERROR :User not found.", clients);
        } 
        else
            sendMessage(client_fd, "ERROR :No nickname provided.", clients);
    }
}