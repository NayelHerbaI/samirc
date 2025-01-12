/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_handling.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 21:54:50 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 09:41:03 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/irc.hpp"

void handleClients(int server_fd, const std::string& server_password) {
    struct pollfd poll_fds[1024];
    std::memset(poll_fds, 0, sizeof(poll_fds));
    poll_fds[0].fd = server_fd;
    poll_fds[0].events = POLLIN;

    int num_fds = 1;
    std::map<int, Client> clients;            // Stores client information
    std::map<int, bool> isAuthenticated;     // Tracks authentication states
    
    std::map<std::string, std::set<int>> channels; // Channel name -> set of client_fds

    std::map<int, time_t> lastActivityTime; // Tracks last activity time for each client
    
    std::map<std::string, std::set<int>> inviteList; // Channel name -> set of invited client FDs

    std::map<std::string, std::set<char>> channelModes; // Channel name -> set of modes

    std::map<std::string, std::string> channelTopics;
    
    std::map<std::string, int> channelUserLimits;       // NEW
    std::map<std::string, std::set<int>> channelOperators;  // NEW
    std::map<std::string, std::string> channelKeys;          // NEW
    
    while (server_running)
    {
        int poll_count = poll(poll_fds, num_fds, 1000);
        if (poll_count < 0)
        {
            perror("poll");
            break;
        }

        for (int i = 0; i < num_fds; ++i)
        {
            if (poll_fds[i].revents & POLLIN)
            {
                if (poll_fds[i].fd == server_fd)
                {
                    // Handle new client connection
                    struct sockaddr_in client_address;
                    socklen_t client_len = sizeof(client_address);
                    int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_len);
                    if (client_fd < 0)
                        perror("accept");
                    else 
                    {
                        std::cout << "\033[32mNew client connected: " << client_fd << std::endl;
                        poll_fds[num_fds].fd = client_fd;
                        poll_fds[num_fds].events = POLLIN;
                        num_fds++;

                        // Initialize the new client
                        clients[client_fd] = Client();
                        isAuthenticated[client_fd] = false; // Client not authenticated yet
                    }
                }
                else
                {
                    // Handle client communication
                    char buffer[512];
                    int bytes_read = read(poll_fds[i].fd, buffer, sizeof(buffer));
                    if (bytes_read <= 0)
                    {
                        std::cout << "\033[31mClient disconnected: " << poll_fds[i].fd << std::endl;
                        close(poll_fds[i].fd);
                        poll_fds[i] = poll_fds[--num_fds];
                        i--;
                        clients.erase(poll_fds[i].fd);         // Remove client
                        isAuthenticated.erase(poll_fds[i].fd); // Remove auth state
                    }
                    else
                    {
                        buffer[bytes_read] = '\0';
                        std::string input(buffer);
                        std::cout << "\033[33mReceived: " << input << " from client " << poll_fds[i].fd << std::endl;

                        // Parse and handle the command
                        std::vector<std::string> command = parseCommand(input);
                        if (!command.empty()) {
                            handleCommand(command, poll_fds[i].fd, server_password, isAuthenticated, clients, channels, inviteList, channelModes, channelTopics, channelUserLimits, channelOperators, channelKeys);
                        }
                    }
                }
            }
        }
    }

    // Close all sockets
    for (int i = 1; i < num_fds; ++i) {
        close(poll_fds[i].fd);
    }
    close(server_fd);
    std::cout << "Server shut down cleanly." << std::endl;
}
