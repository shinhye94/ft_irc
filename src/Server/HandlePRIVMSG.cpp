/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePRIVMSG.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:08:29 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/15 21:14:08 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handlePRIVMSG(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("PRIVMSG", "Handling PRIVMSG command");
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " PRIVMSG :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("PRIVMSG", "Error: Not enough parameters");
		return;
	}
	std::string target = params[0];
	std::string message = "";
	for (size_t i = 1; i < params.size(); ++i) {
		message += params[i] + " ";
	}
	message = Utils::trimString(message);

	std::string senderNickname = getClientNickname(client);

	if (target[0] == '#') { // Channel message
		Channel* channelPtr = findChannel(target);
		if (!channelPtr || !channelPtr->hasUser(client)) {
			sendToClient(client, ":server 404 " + senderNickname + " " + target + " :Cannot send to channel\n"); //ERR_CANNOTSENDTOCHAN or ERR_NOSUCHCHANNEL, ERR_NOTONCHANNEL
			Debug::commandHandling("PRIVMSG", "Error: Cannot send to channel, no such channel or client not in channel: " + target);
			return;
		}
		channelPtr->broadcastMessage(":" + senderNickname + " PRIVMSG " + target + " :" + message + "\n", client);
		Debug::channelEvent("Channel message broadcasted", channelPtr, client);
	} else { // Private message to user
		Client* recipient = findClientByNickname(target);
		if (recipient) {
			 sendToClient(recipient, ":" + senderNickname + " PRIVMSG " + target + " :" + message + "\n");
			 Debug::clientEvent("Private message sent to", recipient);
		} else {
			sendToClient(client, ":server 401 " + senderNickname + " " + target + " :No such nick/channel\n"); //ERR_NOSUCHNICK
			Debug::commandHandling("PRIVMSG", "Error: No such nickname: " + target);
		}
	}
	Debug::commandHandling("PRIVMSG command completed", "");
}
