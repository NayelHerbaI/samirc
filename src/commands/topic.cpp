/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 06:17:36 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/31 18:10:43 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void topicCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients,
    std::map<std::string, std::set<int>>& channels,
    std::map<std::string, std::string>& channelTopics,
    std::map<std::string, std::set<char>>& channelModes, // We'll need to check +t here
    std::map<std::string, std::set<int>>& channelOperators
)
{
    // Basic usage: TOPIC #channel [<new topic>]
    if (command.empty() || command[0] != "TOPIC")
        return; // Not actually a TOPIC command or empty

    // We need at least: TOPIC <channel>
    if (command.size() < 2) {
        sendMessage(client_fd, "ERROR :No channel name given", clients);
        return;
    }

    std::string channel_name = command[1];
                                                                    // /topic #channell this is a new topic
    // Check if the channel exists                                  //  0        1        2  3  4  5   6
    if (channels.find(channel_name) == channels.end()) {
        sendMessage(client_fd, "ERROR :No such channel " + channel_name, clients);
        return;
    }

    // If user typed exactly: TOPIC #channel
    // => They want to see the topic
    if (command.size() == 2) {
        if (!channelTopics[channel_name].empty()) {
            // 332 is the typical IRC numeric for RPL_TOPIC
            sendMessage(client_fd, "332 " + clients[client_fd].nickname 
                                    + " " + channel_name 
                                    + " " + channelTopics[channel_name], 
                                    clients);
        } else {
            // 331 is typical for RPL_NOTOPIC
            sendMessage(client_fd, "331 " + clients[client_fd].nickname 
                                    + " " + channel_name 
                                    + " :No topic is set.", 
                                    clients);
        }
        return;
    }

    // At this point, user wants to set/change the topic:
    // e.g. TOPIC #channel :New Topic
    // Let's parse the new topic from command[2..end]
    std::string newTopic = command[2];

    // >>> Here is where we respect +t <<<
    // If +t is set for this channel, only server operators can set the topic.
    // if (channelModes[channel_name].count('t') > 0) {
    //     // Channel is +t
    //     if (!clients[client_fd].isOperator) {
    //         // The user is not an operator => can't set topic
    //         sendMessage(client_fd, "ERROR :You must be an operator to set the topic on a +t channel.", clients);
    //         return;
    //     }
    // }
    // If the channel is +t, only channel operators can set the topic:
    if (channelModes[channel_name].count('t') > 0) {
        // Check if this user is a channel operator
        if (!channelOperators[channel_name].count(client_fd)) {
            sendMessage(client_fd, "ERR_CHANOPRIVSNEEDED :Only channel operators can set the topic in +t channel", clients);
            return;
        }
    }
    // If +t is NOT set, any user can set the topic (or you can keep the old operator check).

    // Now set the topic
    channelTopics[channel_name] = newTopic;

    // Notify everyone in the channel
    // Format: :<nick>!~<user> TOPIC #channel :<newTopic>
    std::string fullMsg = ":" + clients[client_fd].nickname + " TOPIC " + channel_name + " " + newTopic;
    for (int member_fd : channels[channel_name]) {
        sendMessage(member_fd, fullMsg, clients);
    }

    std::cout << "[INFO] : Topic for channel " << channel_name 
              << " set to: " << newTopic << std::endl;
}
