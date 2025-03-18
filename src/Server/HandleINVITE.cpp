/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleINVITE.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:10:55 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 06:14:52 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleINVITE(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("INVITE", "Handling INVITE command");
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " INVITE :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("INVITE", "Error: Not enough parameters");
		return;
	}
	std::string inviteeNickname = params[0];
	std::string channelName = params[1];

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\r\n"); // ERR_NOSUCHCHANNEL
		Debug::commandHandling("INVITE", "Error: No such channel: " + channelName);
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.hasUser(client)) {
		sendToClient(client, ":server 442 " + getClientNickname(client) + " " + channelName + " :You're not on that channel\r\n"); // ERR_NOTONCHANNEL
		Debug::commandHandling("INVITE", "Error: Not on channel: " + channelName);
		return;
	}

	if (channel.isInviteOnly() && !channel.isOperator(client)) {
		sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\r\n"); // ERR_CHANOPRIVSNEEDED
		Debug::commandHandling("INVITE", "Error: Not channel operator for invite-only channel: " + channelName);
		return;
	}

	Client* inviteeClient = findClientByNickname(inviteeNickname);
	if (!inviteeClient) {
		sendToClient(client, ":server 401 " + getClientNickname(client) + " " + inviteeNickname + " :No such nick/channel\r\n"); // ERR_NOSUCHNICK
		Debug::commandHandling("INVITE", "Error: No such nickname to invite: " + inviteeNickname);
		return;
	}

	if (channel.hasUser(inviteeClient)) {
		sendToClient(client, ":server 443 " + getClientNickname(client) + " " + inviteeNickname + " " + channelName + " :is already on channel\r\n"); // ERR_USERONCHANNEL
		Debug::commandHandling("INVITE", "Error: User already on channel: " + inviteeNickname + ", Channel: " + channelName);
		return;
	}

	channel.inviteUser(inviteeNickname); // Add to invite list.
	sendToClient(client, ":server 341 " + getClientNickname(client) + " " + inviteeNickname + " " + channelName + "\r\n"); //RPL_INVITING
	std::string nickname = getClientNickname(client);
	std::string username = client->getUsername();
	std::string hostname = client->getHostname();
	sendToClient(inviteeClient, ":" + nickname + "!" + username + "@" + hostname + " INVITE "+ inviteeNickname + " " + channelName + "\r\n"); //Send INVITE notice to invited user.
	Debug::channelEvent("Client invited to channel", &channel, inviteeClient);
	Debug::clientEvent("Client invited by", client);
	Debug::commandHandling("INVITE command completed", "Invitee: " + inviteeNickname + ", Channel: " + channelName);
}
