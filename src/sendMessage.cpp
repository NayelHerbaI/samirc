/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sendMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 20:27:07 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/28 02:25:27 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/irc.hpp"

// Send a message to the client
// void sendMessage(int client_fd, const std::string &message)
// {
//     std::string formattedMessage = message + "\r\n"; // IRC messages end with \r\n
//     write(client_fd, formattedMessage.c_str(), formattedMessage.size());
//     std::cout << "Message received to sendMessage is " << message << std::endl;
// }

bool sendMessage(int client_fd, const std::string &message, std::map<int, Client>& clients) {
    if (clients.find(client_fd) == clients.end()) {
        std::cout << "\033[31msendMessage failed: Client " << client_fd << " does not exist." << std::endl;
        return false;
    }
    std::cout << "\033[34mSending message to client " << client_fd << ": " << message << std::endl;
    std::string formattedMessage = message + "\r\n"; // IRC messages end with \r\n
    return write(client_fd, formattedMessage.c_str(), formattedMessage.size()) != -1;
}
