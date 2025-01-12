/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 23:09:31 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/28 23:10:24 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    pingCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients)
{
    if (command[0] == "PING")
    {
        if (command.size() > 1)
        {
            // Respond with PONG using the same token
            sendMessage(client_fd, "PONG :" + command[1], clients);
            std::cout << "\033[35m[INFO] : Received PING, sent PONG to client " << client_fd << std::endl;
        }
        else
            sendMessage(client_fd, "ERROR :PING command requires a parameter", clients);
    }
}