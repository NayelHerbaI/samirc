/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 22:56:45 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 02:45:04 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    passCommand(
    const std::vector<std::string>& command,
    const std::string& server_password,
    std::map<int, bool>& isAuthenticated,
    std::map<int, Client>& clients,
    int client_fd
    )
{
    if (command[0] == "PASS") {
        if (command.size() > 1) {
            if (command[1] == server_password) {
                // Correct password: grant operator privileges
                isAuthenticated[client_fd] = true;
                clients[client_fd].isOperator = true; // Grant operator status
                sendMessage(client_fd, "NOTICE AUTH :Password accepted. You are now an operator.", clients);
                std::cout << "[INFO] Client " << client_fd << " authenticated as an operator." << std::endl;
            } else {
                // Invalid password: allow connection as a regular user
                isAuthenticated[client_fd] = true;
                clients[client_fd].isOperator = false; // Regular user
                sendMessage(client_fd, "NOTICE AUTH :Invalid password. Connected as a regular user.", clients);
                std::cout << "[INFO] Client " << client_fd << " connected as a regular user with an invalid password." << std::endl;
            }
        } else {
            // No password provided: allow as a regular user
            isAuthenticated[client_fd] = true;
            clients[client_fd].isOperator = false; // Regular user
            sendMessage(client_fd, "NOTICE AUTH :No password provided. Connected as a regular user.", clients);
            std::cout << "[INFO] Client " << client_fd << " connected as a regular user with no password." << std::endl;
        }
    } else if (!isAuthenticated[client_fd]) {
        // If not authenticated and trying other commands
        // sendMessage(client_fd, "ERROR :You must authenticate with PASS first.", clients);
        return;
    }
}