/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 23:22:55 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/31 18:27:53 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

// Merged modeCommand supporting +i, -i, +t, -t, +k, -k, +o, -o, +l, -l.
void modeCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients,
    std::map<std::string, std::set<int>>& channels,
    std::map<std::string, std::set<char>>& channelModes,
    std::map<std::string, std::string>& channelKeys,         // NEW for +k
    std::map<std::string, std::set<int>>& channelOperators,  // NEW for +o
    std::map<std::string, int>& channelUserLimits            // NEW for +l
)
{
    if (command.empty() || command[0] != "MODE")
        return; // Not actually a MODE command

    // Only server operators can use MODE in this design
    if (!clients[client_fd].isOperator) {
        sendMessage(client_fd, "ERROR :You must be an operator to use this command.", clients);
        return;
    }

    // We need at least: MODE #channel +something
    if (command.size() < 3) {
        sendMessage(client_fd, "ERROR :MODE command requires a channel and a mode (e.g. +i, -i, +t, -t, +k, -k, +o, -o, +l, -l)", clients);
        return;
    }

    std::string channel_name = command[1];
    std::string modeArg      = command[2];  // e.g. "+i", "-i", "+k", "-k", etc.

    // Check if the channel exists
    if (channels.find(channel_name) == channels.end()) {
        sendMessage(client_fd, "ERROR :Channel " + channel_name + " does not exist.", clients);
        return;
    }

    // Make sure channelModes has an entry for this channel
    if (channelModes.find(channel_name) == channelModes.end()) {
        channelModes[channel_name] = {};
    }

    // Debug: show what mode we’re setting
    std::cout << "[DEBUG] Setting mode \"" << modeArg 
              << "\" on channel " << channel_name << std::endl;

    // We assume modeArg is e.g. "+i", "-i", "+k", etc.
    // Grab the sign (+/-) and the letter (i, t, k, o, l)
    if (modeArg.size() < 2) {
        sendMessage(client_fd, "ERROR :Invalid mode format (must be +x or -x).", clients);
        return;
    }

    char plusOrMinus = modeArg[0];  // '+' or '-'
    char modeChar    = modeArg[1];  // 'i', 't', 'k', 'o', 'l', etc.

    //
    // We’ll handle each mode in a switch or if-else chain
    //
    switch (modeChar)
    {
    //=================================================
    // +i / -i : Invite-only
    //=================================================
    case 'i':
        if (plusOrMinus == '+') {
            if (channelModes[channel_name].count('i') == 0) {
                channelModes[channel_name].insert('i');
                sendMessage(client_fd, "NOTICE AUTH :Invite-only mode (+i) enabled for " + channel_name, clients);
                std::cout << "\033[35m[INFO] : Invite-only mode (+i) enabled for channel " 
                          << channel_name << "\033[0m" << std::endl;
            } else {
                sendMessage(client_fd, "ERROR :Invite-only mode is already enabled for " + channel_name, clients);
            }
        } else { // '-i'
            if (channelModes[channel_name].count('i') > 0) {
                channelModes[channel_name].erase('i');
                sendMessage(client_fd, "NOTICE AUTH :Invite-only mode (-i) disabled for " + channel_name, clients);
                std::cout << "\033[35m[INFO] : Invite-only mode (-i) disabled for channel " 
                          << channel_name << "\033[0m" << std::endl;
            } else {
                sendMessage(client_fd, "ERROR :Invite-only mode is not enabled for " + channel_name, clients);
            }
        }
        break;

    //=================================================
    // +t / -t : Topic-lock
    //=================================================
    case 't':
        if (plusOrMinus == '+') {
            if (channelModes[channel_name].count('t') == 0) {
                channelModes[channel_name].insert('t');
                sendMessage(client_fd, "NOTICE AUTH :Topic-lock mode (+t) enabled for " + channel_name, clients);
                std::cout << "\033[35m[INFO] : Topic-lock mode (+t) enabled for channel " 
                          << channel_name << "\033[0m" << std::endl;
            } else {
                sendMessage(client_fd, "ERROR :Topic-lock mode is already enabled for " + channel_name, clients);
            }
        } else { // '-t'
            if (channelModes[channel_name].count('t') > 0) {
                channelModes[channel_name].erase('t');
                sendMessage(client_fd, "NOTICE AUTH :Topic-lock mode (-t) disabled for " + channel_name, clients);
                std::cout << "\033[35m[INFO] : Topic-lock mode (-t) disabled for channel " 
                          << channel_name << "\033[0m" << std::endl;
            } else {
                sendMessage(client_fd, "ERROR :Topic-lock mode is not enabled for " + channel_name, clients);
            }
        }
        break;

    //=================================================
    // +k / -k : Channel key (password)
    //=================================================
    case 'k':
        if (plusOrMinus == '+') {
            // +k must be followed by the key arg:
            //   MODE #chan +k secretKey
            if (command.size() < 4) {
                sendMessage(client_fd, "ERROR :+k requires a <password> argument", clients);
                return;
            }
            {
                std::string key = command[3];
                channelModes[channel_name].insert('k');
                channelKeys[channel_name] = key;

                sendMessage(client_fd, "NOTICE AUTH :Channel key (+k) set for " + channel_name, clients);
                std::cout << "[INFO] : +k set. key=" << key << " for channel " << channel_name << std::endl;
            }
        }
        else { // '-k'
            if (channelModes[channel_name].count('k') == 0) {
                sendMessage(client_fd, "ERROR :Channel " + channel_name + " is not +k currently.", clients);
            } else {
                channelModes[channel_name].erase('k');
                channelKeys[channel_name].clear();
                sendMessage(client_fd, "NOTICE AUTH :Channel key (-k) removed for " + channel_name, clients);
                std::cout << "[INFO] : -k unset for channel " << channel_name << std::endl;
            }
        }
        break;

    //=================================================
    // +o / -o : Give/take channel operator
    //=================================================
    case 'o':
        if (plusOrMinus == '+') 
        {
            // +o requires a nickname argument: MODE #chan +o someNick
            if (command.size() < 4) 
            {
                sendMessage(client_fd, "ERROR :+o requires a <nickname> argument", clients);
                return;
            }
            {
                std::string targetNick = command[3];
                // Find the user in your clients map
                int targetFd = -1;
                for (auto& [fd, cli] : clients) {
                    if (cli.nickname == targetNick) {
                        targetFd = fd;
                        break;
                    }
                }
                if (targetFd == -1) {
                    sendMessage(client_fd, "ERROR :No such nickname " + targetNick, clients);
                    return;
                }
                // Add them as an operator for this channel
                channelOperators[channel_name].insert(targetFd);

                sendMessage(client_fd, "NOTICE AUTH :" + targetNick + " is now a channel operator in " + channel_name, clients);
                std::cout << "[INFO] : +o " << targetNick << " for channel " << channel_name << std::endl;
            }
        }
        else { // '-o'
            if (command.size() < 4) {
                sendMessage(client_fd, "ERROR :-o requires a <nickname> argument", clients);
                return;
            }
            {
                std::string targetNick = command[3];
                // Find FD
                int targetFd = -1;
                for (auto& [fd, cli] : clients) {
                    if (cli.nickname == targetNick) {
                        targetFd = fd;
                        break;
                    }
                }
                if (targetFd == -1) {
                    sendMessage(client_fd, "ERROR :No such nickname " + targetNick, clients);
                    return;
                }
                // Check if they're currently an op
                if (!channelOperators[channel_name].count(targetFd)) {
                    sendMessage(client_fd, "ERROR :" + targetNick + " is not an operator in " + channel_name, clients);
                    return;
                }
                channelOperators[channel_name].erase(targetFd);

                sendMessage(client_fd, "NOTICE AUTH :" + targetNick + " is no longer a channel operator in " + channel_name, clients);
                std::cout << "[INFO] : -o " << targetNick << " for channel " << channel_name << std::endl;
            }
        }
        break;

    //=================================================
    // +l / -l : Set/remove user limit
    //=================================================
    case 'l':
        if (plusOrMinus == '+') {
            // +l requires a numeric argument: MODE #chan +l 10
            if (command.size() < 4) {
                sendMessage(client_fd, "ERROR :+l requires a numeric <limit>", clients);
                return;
            }
            int limit = std::atoi(command[3].c_str());
            if (limit <= 0) {
                sendMessage(client_fd, "ERROR :+l requires a positive integer limit", clients);
                return;
            }
            channelModes[channel_name].insert('l');
            channelUserLimits[channel_name] = limit;

            sendMessage(client_fd, "NOTICE AUTH :User limit (+l) set to " + std::to_string(limit) + " for " + channel_name, clients);
            std::cout << "[INFO] : +l set to " << limit << " for channel " << channel_name << std::endl;
        }
        else { // '-l'
            if (channelModes[channel_name].count('l') == 0) {
                sendMessage(client_fd, "ERROR :Channel " + channel_name + " is not +l currently.", clients);
            } else {
                channelModes[channel_name].erase('l');
                channelUserLimits[channel_name] = 0; // or erase it if you prefer

                sendMessage(client_fd, "NOTICE AUTH :User limit (-l) removed for " + channel_name, clients);
                std::cout << "[INFO] : -l unset for channel " << channel_name << std::endl;
            }
        }
        break;

    //=================================================
    // Anything else => Invalid mode
    //=================================================
    default:
        sendMessage(client_fd, 
            "ERROR :Invalid mode. Supported modes include: +i, -i, +t, -t, +k, -k, +o, -o, +l, -l.", 
            clients
        );
        break;
    }
}



// void    modeCommand(
//     const std::vector<std::string>& command,
//     int client_fd,
//     std::map<int, Client>& clients,
//     std::map<std::string, std::set<int>>& channels,
//     std::map<std::string, std::set<char>>& channelModes
// )
// {
//     if (command[0] == "MODE") {
//         if (!clients[client_fd].isOperator) {
//             sendMessage(client_fd, "ERROR :You must be an operator to use this command.", clients);
//             return;
//         }

//         if (command.size() < 3) {
//             sendMessage(client_fd, "ERROR :MODE command requires a channel and a mode (+i, -i, +t, -t).", clients);
//             return;
//         }

//         std::string channel_name = command[1];
//         std::string mode = command[2];

//         // Check if the channel exists
//         if (channels.find(channel_name) == channels.end()) {
//             sendMessage(client_fd, "ERROR :Channel " + channel_name + " does not exist.", clients);
//             return;
//         }

//         // Initialize channelModes if not already initialized
//         if (channelModes.find(channel_name) == channelModes.end()) {
//             channelModes[channel_name] = {};
//         }

//         // DEBUG: Show what mode we are setting
//         std::cout << "[DEBUG] Setting mode \"" << mode << "\" on channel " 
//                   << channel_name << std::endl;

//         // Validate and apply the mode
//         if (mode == "+i") {
//             if (channelModes[channel_name].count('i') == 0) {
//                 channelModes[channel_name].insert('i');
//                 sendMessage(client_fd, "NOTICE AUTH :Invite-only mode enabled for " + channel_name, clients);
//                 std::cout << "\033[35m[INFO] : Invite-only mode (+i) enabled for channel " 
//                           << channel_name << "\033[0m" << std::endl;
//             } else {
//                 sendMessage(client_fd, "ERROR :Invite-only mode is already enabled for " + channel_name, clients);
//             }
//         }
//         else if (mode == "-i") {
//             if (channelModes[channel_name].count('i') > 0) {
//                 channelModes[channel_name].erase('i');
//                 sendMessage(client_fd, "NOTICE AUTH :Invite-only mode disabled for " + channel_name, clients);
//                 std::cout << "\033[35m[INFO] : Invite-only mode (-i) disabled for channel " 
//                           << channel_name << "\033[0m" << std::endl;
//             } else {
//                 sendMessage(client_fd, "ERROR :Invite-only mode is not enabled for " + channel_name, clients);
//             }
//         }
//         // BEGIN: Add your +t/-t blocks here
//         else if (mode == "+t") {
//             if (channelModes[channel_name].count('t') == 0) {
//                 channelModes[channel_name].insert('t');
//                 sendMessage(client_fd, "NOTICE AUTH :Topic-lock mode (+t) enabled for " + channel_name, clients);
//                 std::cout << "\033[35m[INFO] : Topic-lock mode (+t) enabled for channel " 
//                           << channel_name << "\033[0m" << std::endl;
//             } else {
//                 sendMessage(client_fd, "ERROR :Topic-lock mode is already enabled for " + channel_name, clients);
//             }
//         }
//         else if (mode == "-t") {
//             if (channelModes[channel_name].count('t') > 0) {
//                 channelModes[channel_name].erase('t');
//                 sendMessage(client_fd, "NOTICE AUTH :Topic-lock mode (-t) disabled for " + channel_name, clients);
//                 std::cout << "\033[35m[INFO] : Topic-lock mode (-t) disabled for channel " 
//                           << channel_name << "\033[0m" << std::endl;
//             } else {
//                 sendMessage(client_fd, "ERROR :Topic-lock mode is not enabled for " + channel_name, clients);
//             }
//         }
//         // END: Add your +t/-t blocks here
//         else {
//             sendMessage(client_fd, "ERROR :Invalid mode. Supported modes: +i, -i, +t, -t.", clients);
//         }
//     }
// }