/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 23:03:29 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/29 18:59:09 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    userCommand(
    const std::vector<std::string>& command,
    const std::string& server_password,
    std::map<int, bool>& isAuthenticated,
    std::map<int, Client>& clients,
    int client_fd)
{
    if (command[0] == "USER") {
        if (command.size() > 1) {
            clients[client_fd].username = command[1];
            sendMessage(client_fd, "NOTICE AUTH :Username set to " + command[1], clients);
            std::cout << "\033[35m[INFO] : Client " << client_fd << " set username to: " << command[1] << std::endl;
    
            // Check if both NICK and USER are set
            if (!clients[client_fd].nickname.empty()) {
                sendMessage(client_fd, ":ircserver 001 " + clients[client_fd].nickname + " :Welcome to the IRC server", clients);
                std::cout << "\033[35m[INFO] : Sent RPL_WELCOME to client " << client_fd << std::endl;
            }
        } else {
            sendMessage(client_fd, "ERROR :No username provided", clients);
        }
    } else if (command[0] == "NICK") {
        if (command.size() > 1) {
            clients[client_fd].nickname = command[1];
            sendMessage(client_fd, "NOTICE AUTH :Nickname set to " + command[1], clients);
            std::cout << "\033[35m[INFO] : Client " << client_fd << " set nickname to: " << command[1] << std::endl;
    
            // Check if both NICK and USER are set
            if (!clients[client_fd].username.empty()) {
                sendMessage(client_fd, ":ircserver 001 " + clients[client_fd].nickname + " :Welcome to the IRC server", clients);
                std::cout << "\033[35m[INFO] : Sent RPL_WELCOME to client " << client_fd << std::endl;
            }
        } else {
            sendMessage(client_fd, "ERROR :No nickname provided", clients);
        }
    }
}