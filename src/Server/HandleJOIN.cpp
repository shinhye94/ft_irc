/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleJOIN.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:07:55 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 09:21:56 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleJOIN(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("JOIN", "Handling JOIN command");
	if (params.empty()) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " JOIN :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("JOIN", "Error: Not enough parameters");
		return;
	}
	std::string channelName = params[0];
	if (channelName[0] != '#') {
		sendToClient(client, ":server 476 " + channelName + " :Invalid channel name\r\n"); // ERR_BADCHANMASK (Simplified error)
		Debug::commandHandling("JOIN", "Error: Invalid channel name format: " + channelName);
		return;
	}

	std::string key = "";
	if (params.size() > 1) {
		key = params[1]; // Channel key if provided in JOIN command
	}

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		_channels.insert(std::pair<std::string, Channel>(channelName, Channel(channelName))); // Create new channel if it doesn't exist. - explicit std::pair
		channelPtr = findChannel(channelName); // Retrieve the newly created channel.
		channelPtr->setJustCreated(true);
		Debug::channelEvent("Channel created", channelPtr);
	}

	Channel& channel = *channelPtr;

	if (channel.isInviteOnly() && !channel.isInvited(getClientNickname(client))) {
		sendToClient(client, ":server 473 " + getClientNickname(client) + " " + channelName + " :Cannot join channel (+i)\r\n"); // ERR_INVITEONLYCHAN
		Debug::commandHandling("JOIN", "Error: Invite-only channel, client not invited: " + channelName);
		return;
	}

	if (channel.hasKey() && !channel.checkKey(key)) {
		sendToClient(client, ":server 475 " + getClientNickname(client) + " " + channelName + " :Cannot join channel (+k)\r\n"); // ERR_BADCHANNELKEY
		Debug::commandHandling("JOIN", "Error: Incorrect channel key: " + channelName);
		return;
	}

	if (!channel.addUser(client)) { //addUser returns false if limit reached or user already in channel
		 if (channel.isLimitReached()) {
			sendToClient(client, ":server 471 " + getClientNickname(client) + " " + channelName + " :Channel is full (+l)\r\n"); // ERR_CHANNELISFULL
			Debug::commandHandling("JOIN", "Error: Channel is full: " + channelName);
			return ;
		 } else if (channel.hasUser(client)) { // Already in channel (should not normally happen from client's perspective in a normal scenario, but handle it).
			 return; // Already in channel, nothing to do.
		 } else { // General error, could extend error codes for more specific reasons if needed.
			 sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\r\n"); // Or more general error code. ERR_NOSUCHCHANNEL is used as fallback.
			 Debug::commandHandling("JOIN", "Error: Failed to join channel (unknown reason): " + channelName);
			 return;
		 }
	}
	if (channel.getUserCount() == 1 && channel.getJustCreated() && channel.hasUser(client))
		channel.addOperator(client);

 	channel.clearInvites(); // Clear invite status upon successful join.
	Debug::channelEvent("Client joined channel", &channel, client);


	std::string nickname = getClientNickname(client);

	// Send JOIN message to the client who joined
	std::string username = client->getUsername();
	std::string hostname = client->getHostname();
	sendToClient(client, ":" + nickname + "!" + username + "@" + hostname + " JOIN "+ channelName + " :" + _currentCommand + "\r\n");

	// Send topic (no topic implemented in this example, send RPL_NOTOPIC)
	sendToClient(client, ":server 331 " + nickname + " " + channelName + " :No topic is set\r\n"); //RPL_NOTOPIC

	// Send NAMES list
	std::string namesList = ":server 353 " + nickname + " = " + channelName + " :"; //RPL_NAMREPLY
	for (std::set<Client*>::iterator user_it = channel.getUsers().begin(); user_it != channel.getUsers().end(); ++user_it) { // Iterator-based loop
		Client* userInChannel = *user_it;
		namesList += getClientNickname(userInChannel) + " ";
	}
	namesList += "\n";
	sendToClient(client, namesList);
	sendToClient(client, ":server 366 " + nickname + " " + channelName + " :End of /NAMES list\r\n"); //RPL_ENDOFNAMES

	// Notify other channel members about the join
	broadcastToChannel(channelName, ":" + nickname + "!" + username + "@" + "127.0.0.1" + " JOIN :"+ channelName + "\r\n", client);
	Debug::channelEvent("JOIN command completed", &channel, client);
}
