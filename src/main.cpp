/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/25 16:07:09 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/29 19:05:32 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/irc.hpp"

bool server_running = true;

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string server_password = argv[2];

    // Set up signal handler
    setupSignalHandler();

    // Initialize server socket
    int server_fd = setupServerSocket(port);
    if (server_fd == -1) {
        std::cerr << "Failed to set up server socket." << std::endl;
        return 1;
    }

    // Start handling clients
    handleClients(server_fd, server_password);

    return 0;
}

// /connect 127.0.0.1/6667 -password=hello
// /server add ircserver 127.0.0.1/6667 -password=hello
// /server del ircserver
// /quote JOIN #testchannel
// /quote USER <username> 0 * :Real Name
// /nick <nickname>
// /quote PASS <password>
// /alias add join /quote JOIN


// how to use ?
// launch ircserver.
// launch weechat
// add the server on weechat with /server add ircserver 127.0.0.1/6667 -password=hello
// can change port by changing 6667 to the one required. Can change password as well. Change hello here.
// setup username and nickname ensure full functionnalities.
// /nick mynickname
// /quote USER myusername 0 * :My Real Name
// /alias add partclose /part $1; /close
// /set logger.file.enabled off
// weechat --dir ~/.weechat2
