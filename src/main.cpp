/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 13:30:41 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 08:56:29 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/Debug.hpp"


#include <iostream>
#include <sstream>


static bool	worngArgs(int argc, char**argv) {
	if (argc != 3) {
		std::cerr << Colors::returnColored(YELLOW, "Warning: Wrong number of arguments!", NEWLINE);
		return (true);
	}
	int	port;
	port = atoi(argv[1]);
	std::string str = argv[1];
	for (size_t i = 0; i < str.length(); i++) {
		if (!isdigit(str[i])) {
			std::cerr << Colors::returnColored(RED, "Error: Port number cannot be somthing rather than digit", NEWLINE);
			return (true);
		}
	}
	if (port <= 2000 || port >= 65000) {
		std::cerr << Colors::returnColored(YELLOW, "Warning: port number is invalid or occupied please \
					use another one above 2000", NEWLINE);
					return (true);
	}
	std::string	oss;
	oss = argv[2];
	for (size_t i = 0; i < oss.length(); i++) {
		if (!isprint(oss[i]) || (oss[i] >= 9 && oss[i] <= 13) || oss[i] == ' ') {
			std::cerr << Colors::returnColored(RED, "Error: Password cannot be something not printable", NEWLINE);
			return (true);
		}
	}
	if (oss.empty()) {
		std::cerr << Colors::returnColored(YELLOW, "Warning: Password cannot be empty", NEWLINE);
		return (true);
	}
	return (false);
}

int	main(int argc, char **argv) {
	// default port and password
	int	port;
	std::string	password;

	// checking all args and other
	if (worngArgs(argc, argv)) {
		std::cerr << Colors::returnColored(RED, "Error: Wrong arguments! ", NEWLINE);
		return (1);
	}
	port = atoi(argv[1]);
	password = argv[2];
	// Constructing the basic server class
	Server	server(port, password);
	// runing the server and doing all the stuff
	Debug::serverPhase("Starting IRC Server...");
	server.run();
	Debug::serverPhase("IRC Server finished.");
	return (0);
}
/**
 * @todo
 * 		* change the base strucut: add a channel.hpp & client.hpp and channel.cpp & client.cpp and implement them
 * instead fo the current _channel in server.cpp
 *
 * 		* adjust socket problem when occupeing port and not releaseing it
 * 		* adjust nickname \n prblem.
 * 		* utilise de rawlog debuging method
 *
 * 		* add more functionnalities:
 * 			- MODE (-t -i -k -o -l) (ommand is used to set channel or user modes. In a channel context)
 * 				* -t:Only channel operators can change the topic.
 * 				* -i:Marks the channel as invite-only.
 * 				* -k:Sets a channel key (password).
 * 				* -o:Gives a user operator status.
 * 				* -l:Limits the number of users who can join the channel.
 *
 * 			- WHOIS (this command retrieves information about a user—such as their real name, channels, idle time, and sign-on time.)
 *
 * 			- PING (this command is typically used by the server to check connectivity, but you can manually send one to test your connection.)
 *
 * 			- KICK (this  command removes a user from a channel. You must be an operator to perform this action.)
 *
 * 			- INVITE (this  command is used to invite a user to join a channel.)
 *
 * 			- TOPIC (this command shows or sets the topic of a channel.)

 */
