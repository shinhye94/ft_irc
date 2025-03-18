/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleKICK.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:10:37 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 06:23:42 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleKICK(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("KICK", "Handling KICK command");
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " KICK :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("KICK", "Error: Not enough parameters");
		return;
	}
	std::string channelName = params[0];
	std::string kickTargetNickname = params[1];
	std::string kickReason = "";
	if (params.size() > 2) {
		for (size_t i = 2; i < params.size(); ++i) {
			kickReason += params[i] + " ";
		}
		kickReason = Utils::trimString(kickReason);
	}

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\r\n"); // ERR_NOSUCHCHANNEL
		Debug::commandHandling("KICK", "Error: No such channel: " + channelName);
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.isOperator(client)) {
		sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\r\n"); // ERR_CHANOPRIVSNEEDED
		Debug::commandHandling("KICK", "Error: Not channel operator for: " + channelName);
		return;
	}

	Client* targetClient = findClientByNickname(kickTargetNickname);
	if (!targetClient || !channel.hasUser(targetClient)) {
		sendToClient(client, ":server 441 " + getClientNickname(client) + " " + kickTargetNickname + " " + channelName + " :They aren't on that channel\r\n"); // ERR_USERNOTINCHANNEL
		Debug::commandHandling("KICK", "Error: Target user not in channel: " + kickTargetNickname + ", Channel: " + channelName);
		return;
	}

	std::string nickname = getClientNickname(client);
	std::string username = client->getUsername();
	std::string hostname = client->getHostname();
	broadcastToChannel(channelName, ":" + nickname + "!" + username + "@" + hostname + " KICK "+ channelName + " " + kickTargetNickname + " :" + _currentCommand + " :" + kickReason + "\r\n");
	sendToClient(targetClient, ":" + getClientNickname(client) + " KICK " + channelName + " " + kickTargetNickname + " :" + _currentCommand + " :" + kickReason + "\r\n"); // Also send to the kicked user
	Debug::channelEvent("Client kicked from channel", &channel, client);

	channel.removeUser(targetClient);
	if (channel.getUserCount() == 0) {
		_channels.erase(channelName); // Remove channel if empty
		Debug::channelEvent("Channel removed as it became empty after kick", &channel);
	}
	Debug::commandHandling("KICK command completed", "Target: " + kickTargetNickname + ", Channel: " + channelName);
}
