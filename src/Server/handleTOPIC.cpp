/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleTOPIC.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:11:11 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 03:48:30 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleTOPIC(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("TOPIC", "Handling TOPIC command");
	if (params.empty()) {
		 sendToClient(client, ":server 461 " + getClientNickname(client) + " TOPIC :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		 Debug::commandHandling("TOPIC", "Error: Not enough parameters");
		return;
	}

	std::string channelName = params[0];
	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\n"); // ERR_NOSUCHCHANNEL
		Debug::commandHandling("TOPIC", "Error: No such channel: " + channelName);
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.hasUser(client)) {
		sendToClient(client, ":server 442 " + getClientNickname(client) + " " + channelName + " :You're not on that channel\n"); // ERR_NOTONCHANNEL
		Debug::commandHandling("TOPIC", "Error: Not on channel: " + channelName);
		return;
	}


	if (params.size() == 1) { // Get topic
		if (channel.getTopic().empty()) {
			sendToClient(client, ":server 331 " + getClientNickname(client) + " " + channelName + " :No topic is set\n"); //RPL_NOTOPIC
			Debug::commandHandling("TOPIC", "Getting topic - No topic set");
		} else {
			sendToClient(client, ":server 332 " + getClientNickname(client) + " " + channelName + " :" + channel.getTopic() + "\n"); //RPL_TOPIC
			Debug::commandHandling("TOPIC", "Getting topic - Topic sent");
		}
	} else { // Set topic
		if (channel.isTopicLocked() && !channel.isOperator(client)) {
			sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\n"); // ERR_CHANOPRIVSNEEDED
			Debug::commandHandling("TOPIC", "Error: Not channel operator to set topic in locked channel: " + channelName);
			return;
		}
		std::string newTopic = "";
		for (size_t i = 1; i < params.size(); ++i) {
			newTopic += params[i] + " ";
		}
		newTopic = Utils::trimString(newTopic);
		channel.setTopic(newTopic);
		std::string nickname = getClientNickname(client);
		std::string username = client->getUsername();
		std::string hostname = client->getHostname();
		broadcastToChannel(channelName, ":" + nickname + "!" + username + "@" + hostname + " TOPIC "+ channelName + " :" + _currentCommand + "\r\n"); // Send TOPIC to all in channel
		Debug::channelEvent("Topic changed", &channel, client);
		Debug::commandHandling("TOPIC", "Topic changed to: " + newTopic);
	}
	Debug::commandHandling("TOPIC command completed", "Channel: " + channelName);
}
