/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleNICK.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:05:48 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/15 21:06:33 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleNICK(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("NICK", "Handling NICK command");
	if (params.empty()) {
		sendToClient(client, ":server 431 * :No nickname given\n"); //ERR_NONICKNAMEGIVEN
		Debug::commandHandling("NICK", "Error: No nickname given");
		return;
	}
	std::string nickname = params[0];


	// nickname = nickname.substr(0, nickname.length() - 1);
	if (isNicknameInUse(nickname)) {
		sendToClient(client, ":server 433 * " + nickname + " :Nickname is already in use\n"); //ERR_NICKNAMEINUSE
		Debug::commandHandling("NICK", "Error: Nickname already in use: " + nickname);
		return;
	}

	std::string oldNickname = getClientNickname(client);
	client->setNickname(nickname);

	if (oldNickname.empty()) { // First NICK command after connection
		sendToClient(client, ":server NOTICE AUTH :*** Your nickname is now " + nickname + "\n");
		Debug::clientEvent("Nickname set", client);
	} else { // Nickname change
		for (std::map<std::string, Channel>::iterator chan_it = _channels.begin(); chan_it != _channels.end(); ++chan_it) { // Iterator-based loop
			Channel& channel = chan_it->second;
			if (channel.hasUser(client)) {
				broadcastToChannel(chan_it->first, ":" + oldNickname + " NICK :" + nickname + "\n", client);
				Debug::channelEvent("NICK change broadcasted to channel", &channel, client);
			}
		}
		sendToClient(client, ":" + oldNickname + " NICK :" + nickname + "\n"); // Notify the user of their own nick change
		Debug::clientEvent("Nickname changed", client);
	}
	if (!client->getUsername().empty() && oldNickname.empty()) { // Check if USER was set before, now complete registration if NICK is set for the first time.
		sendToClient(client, ":server 001 " + nickname + " :Welcome to the IRC server!\n"); //RPL_WELCOME
		sendToClient(client, ":server 002 " + nickname + " :Your host is server\n"); //RPL_YOURHOST
		sendToClient(client, ":server 003 " + nickname + " :This server was created now\n"); //RPL_CREATED
		sendToClient(client, ":server 004 " + nickname + " server - -\n"); //RPL_MYINFO (minimal)
		Debug::clientEvent("Registration completed", client);
	}
}
