/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleUSER.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:07:03 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 07:47:50 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleUSER(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("USER", "Handling USER command");
	if (params.size() < 4) {
		sendToClient(client, ":server 461 * USER :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("USER", "Error: Not enough parameters");
		return;
	}
	std::string username = params[0];
	std::string	hostname = params[1];
	std::string servername = params[2];
	std::string realname = params[3]; // Realname is usually the 4th param and onwards, but we'll just take the 4th for simplicity here.

	client->setNamesAndStuff(username, hostname, servername, realname);

	std::string nickname = getClientNickname(client);
	if (!nickname.empty()) { // Check if NICK was set before, now complete registration if USER is set.
		sendToClient(client, ":server 001 " + nickname + " :Welcome to the IRC server!\r\n"); //RPL_WELCOME
		sendToClient(client, ":server 002 " + nickname + " :Your host is " + servername + "\r\n"); //RPL_YOURHOST
		sendToClient(client, ":server 003 " + nickname + " :This server was created now\r\n"); //RPL_CREATED
		sendToClient(client, ":server 004 " + nickname + " " + servername + "\r\n"); //RPL_MYINFO (minimal)
		Debug::clientEvent("Registration completed after USER", client);
	} else {
		 Debug::clientEvent("Username set", client);
	}
}
