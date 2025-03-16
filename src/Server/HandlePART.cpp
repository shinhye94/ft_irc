/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePART.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:08:11 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/16 03:48:46 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handlePART(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("PART", "Handling PART command");
	if (params.empty()) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " PART :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("PART", "Error: Not enough parameters");
		return;
	}
	std::string channelName = params[0];

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr || !channelPtr->hasUser(client)) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\n"); //ERR_NOSUCHCHANNEL or ERR_NOTONCHANNEL
		Debug::commandHandling("PART", "Error: No such channel or client not in channel: " + channelName);
		return;
	}
	Channel& channel = *channelPtr;


	channel.removeUser(client);
	std::string nickname = getClientNickname(client);
	std::string username = client->getUsername();

	broadcastToChannel(channelName, ":" + nickname + " PART " + channelName + "\n", client);
	// sendToClient(client, ":" + nickname + " PART " + channelName + "\n");
	sendToClient(client, ":" + nickname + "!" + username + "@server" + " PART " + channelName + "\n");
	Debug::channelEvent("Client parted channel", &channel, client);

	if (channel.getUserCount() == 0) {
		// _channels.erase(channelName); // Remove channel if empty
		channel.setRemoveSituation(true);
		Debug::channelEvent("Channel removed as it became empty", &channel);
	}
	Debug::channelEvent("PART command completed", &channel, client);
}
