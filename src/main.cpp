/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 13:30:41 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 01:31:37 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

#include <iostream>
#include <sstream>

int	main(void) {
	int	port = 5557;
	std::string	password = "password123";

	Server	server(port, password);
	server.run();
	return (0);
}

/**
 * @todo
 * 		* change the base strucut: add a channel.hpp & client.hpp and channel.cpp & client.cpp and implement them
 * instead fo the current _channel in server.cpp
 *
 * 		* adjust socket problem when occupeing port and not releaseing it
 * 		* adjust nickname \r\n prblem.
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
