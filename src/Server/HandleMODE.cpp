/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleMODE.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:09:09 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/15 21:15:39 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleMODE(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("MODE", "Handling MODE command");
	if (params.empty()) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\n"); // ERR_NEEDMOREPARAMS
		Debug::commandHandling("MODE", "Error: Not enough parameters");
		return;
	}

	std::string target = params[0];
	if (target[0] == '#') { // Channel MODE
		Channel* channelPtr = findChannel(target);
		if (!channelPtr) {
			sendToClient(client, ":server 403 " + getClientNickname(client) + " " + target + " :No such channel\n"); // ERR_NOSUCHCHANNEL
			Debug::commandHandling("MODE", "Error: No such channel: " + target);
			return;
		}
		Channel& channel = *channelPtr;

		if (!channel.isOperator(client)) {
			sendToClient(client, ":server 482 " + getClientNickname(client) + " " + target + " :You're not channel operator\n"); // ERR_CHANOPRIVSNEEDED
			Debug::commandHandling("MODE", "Error: Not channel operator for: " + target);
			return;
		}

		if (params.size() < 2) {
			sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\n"); // ERR_NEEDMOREPARAMS
			Debug::commandHandling("MODE", "Error: Mode parameter missing for channel: " + target);
			return;
		}

		std::string modeChanges = params[1];
		std::vector<std::string> modeParams;
		if (params.size() > 2) {
			modeParams.assign(params.begin() + 2, params.end());
		}

		size_t paramIndex = 0;
		bool adding = true; // Assume adding mode at start

		for (size_t i = 0; i < modeChanges.length(); ++i) { // Regular for loop
			char modeChar = modeChanges[i];
			if (modeChar == '+') {
				adding = true;
			} else if (modeChar == '-') {
				adding = false;
			} else if (modeChar == 't') {
				channel.setTopicLocked(adding);
				broadcastToChannel(target, ":server MODE " + target + " " + (adding ? "+t" : "-t") + "\n");
				Debug::channelEvent("Topic lock mode changed", &channel, client);
			} else if (modeChar == 'i') {
				channel.setInviteOnly(adding);
				broadcastToChannel(target, ":server MODE " + target + " " + (adding ? "+i" : "-i") + "\n");
				Debug::channelEvent("Invite-only mode changed", &channel, client);
			} else if (modeChar == 'k') {
				if (adding) {
					if (paramIndex < modeParams.size()) {
						channel.setKey(modeParams[paramIndex++]);
						broadcastToChannel(target, ":server MODE " + target + " +k " + channel.getKey() + "\n");
						Debug::channelEvent("Channel key set", &channel, client);
					} else {
						sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\n"); // ERR_NEEDMOREPARAMS for key
						Debug::commandHandling("MODE", "Error: Key parameter missing for +k mode");
						return;
					}
				} else {
					channel.clearKey();
					broadcastToChannel(target, ":server MODE " + target + " -k\n");
					Debug::channelEvent("Channel key cleared", &channel, client);
				}
			} else if (modeChar == 'o') {
				if (paramIndex < modeParams.size()) {
					Client* targetClient = findClientByNickname(modeParams[paramIndex++]);
					if (targetClient && channel.hasUser(targetClient)) {
						if (adding) {
							channel.addOperator(targetClient);
							broadcastToChannel(target, ":server MODE " + target + " +o " + getClientNickname(targetClient) + "\n");
							Debug::channelEvent("Operator status given to user", &channel, client);
						} else {
							channel.removeOperator(targetClient);
							broadcastToChannel(target, ":server MODE " + target + " -o " + getClientNickname(targetClient) + "\n");
							Debug::channelEvent("Operator status removed from user", &channel, client);
						}
					} else {
						sendToClient(client, ":server 441 " + getClientNickname(client) + " " + (targetClient ? getClientNickname(targetClient) : modeParams[paramIndex-1]) + " " + target + " :They aren't on that channel\n"); // ERR_USERNOTINCHANNEL
						Debug::commandHandling("MODE", "Error: Target user not in channel for operator mode change");
						return;
					}
				} else {
					sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\n"); // ERR_NEEDMOREPARAMS for operator nick
					Debug::commandHandling("MODE", "Error: Target nickname missing for operator mode change");
					return;
				}
			} else if (modeChar == 'l') {
				if (adding) {
					if (paramIndex < modeParams.size()) {
						try {
							int limit = atoi(modeParams[paramIndex++].c_str()); // Using atoi for C++98 compatibility
							if (limit > 0) {
								channel.setUserLimit(limit);
								broadcastToChannel(target, ":server MODE " + target + " +l " + Debug::intToString(limit) + "\n"); // Use intToString
								Debug::channelEvent("User limit set", &channel, client);
							} else {
								sendToClient(client, ":server 501 " + getClientNickname(client) + " :Unknown MODE flag\n"); // ERR_UMODEUNKNOWNFLAG - Using this as no ERR_INVALIDMODEPARAM value.
								Debug::commandHandling("MODE", "Error: Invalid user limit value (must be positive)");
								return;
							}
						} catch (const std::invalid_argument& e) {
							sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Invalid parameter for +l mode\n"); // ERR_NEEDMOREPARAMS (could be more specific)
							Debug::commandHandling("MODE", "Error: Invalid parameter for +l mode (not a number)");
							return;
						}
					} else {
						sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\n"); // ERR_NEEDMOREPARAMS for limit
						Debug::commandHandling("MODE", "Error: Limit value missing for +l mode");
						return;
					}
				} else {
					channel.clearUserLimit();
					broadcastToChannel(target, ":server MODE " + target + " -l\n");
					Debug::channelEvent("User limit cleared", &channel, client);
				}
			} else {
				sendToClient(client, ":server 501 " + getClientNickname(client) + " :Unknown MODE flag\n"); // ERR_UMODEUNKNOWNFLAG
				Debug::commandHandling("MODE", "Error: Unknown mode flag: " + std::string(1, modeChar));
				return;
			}
		}
		Debug::channelEvent("MODE command completed", &channel, client);


	} else { // User MODE (Not implemented in this simplified version, inform client of error)
		sendToClient(client, ":server 502 " + getClientNickname(client) + " :Users don't have modes\n"); // ERR_UMODEUNKNOWNFLAG - Misusing this, should be ERR_NOSUCHMODE really for user modes.
		Debug::commandHandling("MODE", "Error: User mode change requested, not implemented");
		return;
	}
}
