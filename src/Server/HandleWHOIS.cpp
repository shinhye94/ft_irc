/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleWHOIS.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:09:49 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/15 21:15:58 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleWHOIS(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("WHOIS", "Handling WHOIS command");
	if (params.empty()) {
		sendToClient(client, ":server 431 * WHOIS :No nickname given\n"); //ERR_NONICKNAMEGIVEN
		Debug::commandHandling("WHOIS", "Error: No nickname provided");
		return;
	}
	std::string targetNickname = params[0];
	Client* targetClient = findClientByNickname(targetNickname);

	if (!targetClient) {
		sendToClient(client, ":server 401 " + getClientNickname(client) + " " + targetNickname + " :No such nick/channel\n"); //ERR_NOSUCHNICK
		sendToClient(client, ":server 318 " + getClientNickname(client) + " " + targetNickname + " :End of WHOIS list\n"); //RPL_ENDOFWHOIS
		Debug::commandHandling("WHOIS", "Error: No such nickname: " + targetNickname);
		return;
	}

	std::string nickname = getClientNickname(client);
	sendToClient(client, ":server 311 " + nickname + " " + targetNickname + " " + targetClient->getUsername() + " server " + _serverPassword + " :" + targetClient->getUsername() + "\n"); //RPL_WHOISUSER (simplified)
	std::string channelsList = ":server 319 " + nickname + " " + targetNickname + " :"; //RPL_WHOISCHANNELS
	for (std::map<std::string, Channel>::iterator chan_it = _channels.begin(); chan_it != _channels.end(); ++chan_it) { // Iterator-based loop
		Channel& channel = chan_it->second;
		if (channel.hasUser(targetClient)) {
			channelsList += chan_it->first + " ";
		}
	}
	channelsList += "\n";
	sendToClient(client, channelsList);
	sendToClient(client, ":server 318 " + nickname + " " + targetNickname + " :End of WHOIS list\n"); //RPL_ENDOFWHOIS
	Debug::commandHandling("WHOIS command completed", "Nickname: " + targetNickname);
}
