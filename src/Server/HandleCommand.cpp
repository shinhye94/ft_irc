/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleCommand.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 10:02:21 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 12:28:18 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"


void	Server::processCommand(Client* client, const std::string& commandLine) {
	std::vector<std::string> parts = Utils::splitString(commandLine, " ");
	if (parts.empty()) return;

	std::string command = parts[0];
	std::vector<std::string> params;
	if (parts.size() > 1) {
		params.assign(parts.begin() + 1, parts.end());
	}

	if (command == "PASS") {
		handlePASS(client, params);
	} else if (!isClientAuthenticated(client)) {
		sendToClient(client, ":server 451 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " :You have not registered\r\n"); // ERR_NOTREGISTERED
		return; // Ignore further commands until authenticated.
	} else if (command == "NICK") {
		handleNICK(client, params);
	} else if (command == "USER") {
		handleUSER(client, params);
	} else if (command == "JOIN") {
		handleJOIN(client, params);
	} else if (command == "PART") {
		handlePART(client, params);
	} else if (command == "PRIVMSG") {
		handlePRIVMSG(client, params);
	} else if (command == "QUIT") {
		handleQUIT(client);
	} else if (command == "MODE") {
		handleMODE(client, params);
	} else if (command == "WHOIS") {
		handleWHOIS(client, params);
	} else if (command == "WHO") {
		handleWHO(client, params);
	} else if (command == "PING") {
		handlePING(client, params);
	} else if (command == "KICK") {
		handleKICK(client, params);
	} else if (command == "INVITE") {
		handleINVITE(client, params);
	} else if (command == "TOPIC") {
		handleTOPIC(client, params);
	}
	else {
		sendToClient(client, ":server 421 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " " + command + " :Unknown command\r\n"); //ERR_UNKNOWNCOMMAND
	}
}

void	Server::handlePASS(Client* client, const std::vector<std::string>& params) {
	if (params.empty()) {
		sendToClient(client, ":server 461 * PASS :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		return;
	}
	if (authenticateClient(client, params[0])) {
		client->setAuthenticated(true);
		sendToClient(client, ":server NOTICE AUTH :*** Password accepted.\r\n");
	} else {
		sendToClient(client, ":server 464 * :Password incorrect\r\n"); //ERR_PASSWDMISMATCH
		removeClient(client);
	}
}

void	Server::handleNICK(Client* client, const std::vector<std::string>& params) {
	if (params.empty()) {
		sendToClient(client, ":server 431 * :No nickname given\r\n"); //ERR_NONICKNAMEGIVEN
		return;
	}
	std::string nickname = params[0];
	if (isNicknameInUse(nickname)) {
		sendToClient(client, ":server 433 * " + nickname + " :Nickname is already in use\r\n"); //ERR_NICKNAMEINUSE
		return;
	}

	std::string oldNickname = getClientNickname(client);
	client->setNickname(nickname);

	if (oldNickname.empty()) { // First NICK command after connection
		sendToClient(client, ":server NOTICE AUTH :*** Your nickname is now " + nickname + "\r\n");
	} else { // Nickname change
		std::map<std::string, Channel>::iterator it;
		for (it = _channels.begin(); it != _channels.end(); it++) {
			const std::string&	channelName = it->first;
			Channel&			channel = it->second;
			if (channel.hasUser(client)) {
				broadcastToChannel(channelName, ":" + oldNickname + " NICK :" + nickname + "\r\n", client);
			}
		}
		sendToClient(client, ":" + oldNickname + " NICK :" + nickname + "\r\n"); // Notify the user of their own nick change
	}
	if (!client->getUsername().empty() && oldNickname.empty()) {
		sendToClient(client, ":server 001 " + nickname + " :Welcome to the IRC server!\r\n"); //RPL_WELCOME
		sendToClient(client, ":server 002 " + nickname + " :Your host is server\r\n"); //RPL_YOURHOST
		sendToClient(client, ":server 003 " + nickname + " :This server was created now\r\n"); //RPL_CREATED
		sendToClient(client, ":server 004 " + nickname + " server - -\r\n"); //RPL_MYINFO (minimal)
	}
}

void	Server::handleUSER(Client* client, const std::vector<std::string>& params) {
	if (params.size() < 4) {
		sendToClient(client, ":server 461 * USER :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		return;
	}
	std::string username = params[0];
	std::string realname = params[3];

	client->setUsername(username);

	std::string nickname = getClientNickname(client);
	if (!nickname.empty()) { // Check if NICK was set before, now complete registration if USER is set.
		sendToClient(client, ":server 001 " + nickname + " :Welcome to the IRC server!\r\n"); //RPL_WELCOME
		sendToClient(client, ":server 002 " + nickname + " :Your host is server\r\n"); //RPL_YOURHOST
		sendToClient(client, ":server 003 " + nickname + " :This server was created now\r\n"); //RPL_CREATED
		sendToClient(client, ":server 004 " + nickname + " server - -\r\n"); //RPL_MYINFO (minimal)
	}
}

void	Server::handleJOIN(Client* client, const std::vector<std::string>& params) {
	if (params.empty()) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " JOIN :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		return;
	}
	std::string channelName = params[0];
	if (channelName[0] != '#') {
		sendToClient(client, ":server 476 " + channelName + " :Invalid channel name\r\n"); // ERR_BADCHANMASK
		return;
	}

	std::string key = "";
	if (params.size() > 1) {
		key = params[1]; // Channel key if provided in JOIN command
	}

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		_channels.insert(std::pair<std::string, Channel>(channelName, Channel(channelName))); // Create new channel if it doesn't exist.
		channelPtr = findChannel(channelName); // Retrieve the newly created channel.
	}

	Channel& channel = *channelPtr;

	if (channel.isInviteOnly() && !channel.isInvited(getClientNickname(client))) {
		sendToClient(client, ":server 473 " + getClientNickname(client) + " " + channelName + " :Cannot join channel (+i)\r\n"); // ERR_INVITEONLYCHAN
		return;
	}

	if (channel.hasKey() && !channel.checkKey(key)) {
		sendToClient(client, ":server 475 " + getClientNickname(client) + " " + channelName + " :Cannot join channel (+k)\r\n"); // ERR_BADCHANNELKEY
		return;
	}

	if (!channel.addUser(client)) { //addUser returns false if limit reached or user already in channel
		 if (channel.isLimitReached()) {
			sendToClient(client, ":server 471 " + getClientNickname(client) + " " + channelName + " :Channel is full (+l)\r\n"); // ERR_CHANNELISFULL
		 } else if (channel.hasUser(client)) { // Already in channel (should not normally happen from client's perspective in a normal scenario, but handle it).
			 return; // Already in channel, nothing to do.
		 } else { // General error, could extend error codes for more specific reasons if needed.
			 sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\r\n"); // Or more general error code. ERR_NOSUCHCHANNEL is used as fallback.
			 return;
		 }
	}
	channel.clearInvites(); // Clear invite status upon successful join.

	std::string nickname = getClientNickname(client);

	// Send JOIN message to the client who joined
	sendToClient(client, ":" + nickname + " JOIN " + channelName + "\r\n");

	// Send topic (no topic implemented in this example, send RPL_NOTOPIC)
	sendToClient(client, ":server 331 " + nickname + " " + channelName + " :No topic is set\r\n"); //RPL_NOTOPIC

	// Send NAMES list
	std::string namesList = ":server 353 " + nickname + " = " + channelName + " :"; //RPL_NAMREPLY

	std::set<Client *> tmpUsers = channel.getUsers();
	std::set<Client*>::iterator it;
	for (it = tmpUsers.begin(); it != tmpUsers.end(); it++) {
		Client* userInChannel = *it;
		namesList += getClientNickname(userInChannel) + " ";
	}
	namesList += "\r\n";
	sendToClient(client, namesList);
	sendToClient(client, ":server 366 " + nickname + " " + channelName + " :End of /NAMES list\r\n"); //RPL_ENDOFNAMES

	// Notify other channel members about the join
	broadcastToChannel(channelName, ":" + nickname + " JOIN " + channelName + "\r\n", client);
}


void	Server::handlePART(Client* client, const std::vector<std::string>& params) {
	if (params.empty()) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " PART :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		return;
	}
	std::string channelName = params[0];

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr || !channelPtr->hasUser(client)) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\r\n"); //ERR_NOSUCHCHANNEL or ERR_NOTONCHANNEL
		return;
	}
	Channel& channel = *channelPtr;


	channel.removeUser(client);
	std::string nickname = getClientNickname(client);

	broadcastToChannel(channelName, ":" + nickname + " PART " + channelName + "\r\n", client);
	sendToClient(client, ":" + nickname + " PART " + channelName + "\r\n");

	if (channel.getUserCount() == 0) {
		_channels.erase(channelName); // Remove channel if empty
	}
}


void	Server::handlePRIVMSG(Client* client, const std::vector<std::string>& params) {
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " PRIVMSG :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
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
			sendToClient(client, ":server 404 " + senderNickname + " " + target + " :Cannot send to channel\r\n"); //ERR_CANNOTSENDTOCHAN or ERR_NOSUCHCHANNEL, ERR_NOTONCHANNEL
			return;
		}
		channelPtr->broadcastMessage(":" + senderNickname + " PRIVMSG " + target + " :" + message + "\r\n", client);
	} else { // Private message to user
		Client* recipient = findClientByNickname(target);
		if (recipient) {
			 sendToClient(recipient, ":" + senderNickname + " PRIVMSG " + target + " :" + message + "\r\n");
		} else {
			sendToClient(client, ":server 401 " + senderNickname + " " + target + " :No such nick/channel\r\n"); //ERR_NOSUCHNICK
		}
	}
}

void	Server::handleQUIT(Client* client) {
	sendToClient(client, ":server NOTICE :Closing connection\r\n");
	removeClient(client);
}

void	Server::handleMODE(Client* client, const std::vector<std::string>& params) {
	if (params.empty()) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS
		return;
	}

	std::string target = params[0];
	if (target[0] == '#') { // Channel MODE
		Channel* channelPtr = findChannel(target);
		if (!channelPtr) {
			sendToClient(client, ":server 403 " + getClientNickname(client) + " " + target + " :No such channel\r\n"); // ERR_NOSUCHCHANNEL
			return;
		}
		Channel& channel = *channelPtr;

		if (!channel.isOperator(client)) {
			sendToClient(client, ":server 482 " + getClientNickname(client) + " " + target + " :You're not channel operator\r\n"); // ERR_CHANOPRIVSNEEDED
			return;
		}

		if (params.size() < 2) {
			sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS
			return;
		}

		std::string modeChanges = params[1];
		std::vector<std::string> modeParams;
		if (params.size() > 2) {
			modeParams.assign(params.begin() + 2, params.end());
		}

		size_t paramIndex = 0;
		bool adding = true; // Assume adding mode at start

		std::string::iterator it;
		for (it = modeChanges.begin(); it != modeChanges.end(); it++) {
			char modeChar = *it;
			if (modeChar == '+') {
				adding = true;
			} else if (modeChar == '-') {
				adding = false;
			} else if (modeChar == 't') {
				channel.setTopicLocked(adding);
				broadcastToChannel(target, ":server MODE " + target + " " + (adding ? "+t" : "-t") + "\r\n");
			} else if (modeChar == 'i') {
				channel.setInviteOnly(adding);
				broadcastToChannel(target, ":server MODE " + target + " " + (adding ? "+i" : "-i") + "\r\n");
			} else if (modeChar == 'k') {
				if (adding) {
					if (paramIndex < modeParams.size()) {
						channel.setKey(modeParams[paramIndex++]);
						broadcastToChannel(target, ":server MODE " + target + " +k " + channel.getKey() + "\r\n");
					} else {
						sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS for key
						return;
					}
				} else {
					channel.clearKey();
					broadcastToChannel(target, ":server MODE " + target + " -k\r\n");
				}
			} else if (modeChar == 'o') {
				if (paramIndex < modeParams.size()) {
					Client* targetClient = findClientByNickname(modeParams[paramIndex++]);
					if (targetClient && channel.hasUser(targetClient)) {
						if (adding) {
							channel.addOperator(targetClient);
							broadcastToChannel(target, ":server MODE " + target + " +o " + getClientNickname(targetClient) + "\r\n");
						} else {
							channel.removeOperator(targetClient);
							broadcastToChannel(target, ":server MODE " + target + " -o " + getClientNickname(targetClient) + "\r\n");
						}
					} else {
						sendToClient(client, ":server 441 " + getClientNickname(client) + " " + (targetClient ? getClientNickname(targetClient) : modeParams[paramIndex-1]) + " " + target + " :They aren't on that channel\r\n"); // ERR_USERNOTINCHANNEL
						return;
					}
				} else {
					sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS for operator nick
					return;
				}
			} else if (modeChar == 'l') {
				if (adding) {
					if (paramIndex < modeParams.size()) {
						try {
							int limit = Utils::parseStringToInt(modeParams[paramIndex++]);
							if (limit > 0) {
								channel.setUserLimit(limit);
								std::ostringstream	oss;
								oss << limit;
								broadcastToChannel(target, ":server MODE " + target + " +l " + oss.str() + "\r\n");
							} else {
								sendToClient(client, ":server 501 " + getClientNickname(client) + " :Unknown MODE flag\r\n"); // ERR_UMODEUNKNOWNFLAG - Using this as no ERR_INVALIDMODEPARAM value.
								return;
							}
						} catch (const std::invalid_argument& e) {
							sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Invalid parameter for +l mode\r\n"); // ERR_NEEDMOREPARAMS (could be more specific)
							return;
						}
					} else {
						sendToClient(client, ":server 461 " + getClientNickname(client) + " MODE :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS for limit
						return;
					}
				} else {
					channel.clearUserLimit();
					broadcastToChannel(target, ":server MODE " + target + " -l\r\n");
				}
			} else {
				sendToClient(client, ":server 501 " + getClientNickname(client) + " :Unknown MODE flag\r\n"); // ERR_UMODEUNKNOWNFLAG
				return;
			}
		}


	} else { // User MODE (Not implemented in this simplified version, inform client of error)
		sendToClient(client, ":server 502 " + getClientNickname(client) + " :Users don't have modes\r\n"); // ERR_UMODEUNKNOWNFLAG - Misusing this, should be ERR_NOSUCHMODE really for user modes.
		return;
	}
}

void	Server::handleWHOIS(Client* client, const std::vector<std::string>& params) {
	if (params.empty()) {
		sendToClient(client, ":server 431 * WHOIS :No nickname given\r\n"); //ERR_NONICKNAMEGIVEN
		return;
	}
	std::string targetNickname = params[0];
	Client* targetClient = findClientByNickname(targetNickname);

	if (!targetClient) {
		sendToClient(client, ":server 401 " + getClientNickname(client) + " " + targetNickname + " :No such nick/channel\r\n"); //ERR_NOSUCHNICK
		sendToClient(client, ":server 318 " + getClientNickname(client) + " " + targetNickname + " :End of WHOIS list\r\n"); //RPL_ENDOFWHOIS
		return;
	}

	std::string nickname = getClientNickname(client);
	sendToClient(client, ":server 311 " + nickname + " " + targetNickname + " " + targetClient->getUsername() + " server " + _serverPassword + " :" + targetClient->getUsername() + "\r\n"); //RPL_WHOISUSER (simplified)
	std::string channelsList = ":server 319 " + nickname + " " + targetNickname + " :"; //RPL_WHOISCHANNELS
	std::map<std::string, Channel>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++) {
		const std::string&	channelName = it->first;
		Channel&			channel = it->second;
		if (channel.hasUser(targetClient)) {
			channelsList += channelName + " ";
		}
	}
	channelsList += "\r\n";
	sendToClient(client, channelsList);
	sendToClient(client, ":server 318 " + nickname + " " + targetNickname + " :End of WHOIS list\r\n"); //RPL_ENDOFWHOIS
}

void	Server::handleWHO(Client* client, const std::vector<std::string>& params) {
	std::string target = "*"; // Default target is all users
	if (!params.empty()) {
		target = params[0]; // Could be a channel or a mask. (Simplified to just channel or all for now).
	}

	std::string nickname = getClientNickname(client);
	/**
	 *
	std::set<Client *> tmpUsers = channel.getUsers();
	std::set<Client*>::iterator it = tmpUsers.begin();
	for (it; it != tmpUsers.end(); it++) {
		Client* userInChannel = *it;
	 */
	if (target[0] == '#') { // WHO for a channel.
		Channel* channelPtr = findChannel(target);
		if (channelPtr) {
			std::set<Client *> tmpUsers =  channelPtr->getUsers();
			std::set<Client*>::iterator it;
			for (it = tmpUsers.begin(); it != tmpUsers.end(); it++) {
				Client* channelClient = *it;
				sendToClient(client, ":server 352 " + nickname + " " + target + " " + channelClient->getUsername() + " server " + _serverPassword + " " + getClientNickname(channelClient) + " H :0 " + channelClient->getUsername() + "\r\n"); //RPL_WHOREPLY (simplified)
			}
		}
	} else { // WHO for all users. (Simplified - no mask support for now)
		std::set<Client*>::iterator iter;
		for (iter = _clients.begin(); iter != _clients.end(); iter++) {
			Client* listClient = *iter;
			 sendToClient(client, ":server 352 " + nickname + " * " + listClient->getUsername() + " server " + _serverPassword + " " + getClientNickname(listClient) + " H :0 " + listClient->getUsername() + "\r\n"); //RPL_WHOREPLY (simplified)
		}
	}
	sendToClient(client, ":server 315 " + nickname + " " + target + " :End of WHO list\r\n"); //RPL_ENDOFWHO
}

void	Server::handlePING(Client* client, const std::vector<std::string>& params) {
	std::string pingMessage = "server"; // Default PING message if none provided.
	if (!params.empty()) {
		pingMessage = params[0]; // Take the first parameter as ping message.
	}
	sendToClient(client, ":server PONG " + pingMessage + "\r\n");
}

void	Server::handleKICK(Client* client, const std::vector<std::string>& params) {
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " KICK :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
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
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.isOperator(client)) {
		sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\r\n"); // ERR_CHANOPRIVSNEEDED
		return;
	}

	Client* targetClient = findClientByNickname(kickTargetNickname);
	if (!targetClient || !channel.hasUser(targetClient)) {
		sendToClient(client, ":server 441 " + getClientNickname(client) + " " + kickTargetNickname + " " + channelName + " :They aren't on that channel\r\n"); // ERR_USERNOTINCHANNEL
		return;
	}

	broadcastToChannel(channelName, ":" + getClientNickname(client) + " KICK " + channelName + " " + kickTargetNickname + " :" + kickReason + "\r\n");
	sendToClient(targetClient, ":" + getClientNickname(client) + " KICK " + channelName + " " + kickTargetNickname + " :" + kickReason + "\r\n"); // Also send to the kicked user

	channel.removeUser(targetClient);
	if (channel.getUserCount() == 0) {
		_channels.erase(channelName); // Remove channel if empty
	}
}

void	Server::handleINVITE(Client* client, const std::vector<std::string>& params) {
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " INVITE :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		return;
	}
	std::string inviteeNickname = params[0];
	std::string channelName = params[1];

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\r\n"); // ERR_NOSUCHCHANNEL
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.hasUser(client)) {
		sendToClient(client, ":server 442 " + getClientNickname(client) + " " + channelName + " :You're not on that channel\r\n"); // ERR_NOTONCHANNEL
		return;
	}

	if (channel.isInviteOnly() && !channel.isOperator(client)) {
		sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\r\n"); // ERR_CHANOPRIVSNEEDED
		return;
	}

	Client* inviteeClient = findClientByNickname(inviteeNickname);
	if (!inviteeClient) {
		sendToClient(client, ":server 401 " + getClientNickname(client) + " " + inviteeNickname + " :No such nick/channel\r\n"); // ERR_NOSUCHNICK
		return;
	}

	if (channel.hasUser(inviteeClient)) {
		sendToClient(client, ":server 443 " + getClientNickname(client) + " " + inviteeNickname + " " + channelName + " :is already on channel\r\n"); // ERR_USERONCHANNEL
		return;
	}

	channel.inviteUser(inviteeNickname); // Add to invite list.
	sendToClient(client, ":server 341 " + getClientNickname(client) + " " + inviteeNickname + " " + channelName + "\r\n"); //RPL_INVITING
	sendToClient(inviteeClient, ":" + getClientNickname(client) + " INVITE " + inviteeNickname + " :" + channelName + "\r\n"); //Send INVITE notice to invited user.
}


void	Server::handleTOPIC(Client* client, const std::vector<std::string>& params) {
	if (params.empty()) {
		 sendToClient(client, ":server 461 " + getClientNickname(client) + " TOPIC :Not enough parameters\r\n"); //ERR_NEEDMOREPARAMS
		return;
	}

	std::string channelName = params[0];
	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\r\n"); // ERR_NOSUCHCHANNEL
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.hasUser(client)) {
		sendToClient(client, ":server 442 " + getClientNickname(client) + " " + channelName + " :You're not on that channel\r\n"); // ERR_NOTONCHANNEL
		return;
	}


	if (params.size() == 1) { // Get topic
		if (channel.getTopic().empty()) {
			sendToClient(client, ":server 331 " + getClientNickname(client) + " " + channelName + " :No topic is set\r\n"); //RPL_NOTOPIC
		} else {
			sendToClient(client, ":server 332 " + getClientNickname(client) + " " + channelName + " :" + channel.getTopic() + "\r\n"); //RPL_TOPIC
		}
	} else { // Set topic
		if (channel.isTopicLocked() && !channel.isOperator(client)) {
			sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\r\n"); // ERR_CHANOPRIVSNEEDED
			return;
		}
		std::string newTopic = "";
		for (size_t i = 1; i < params.size(); ++i) {
			newTopic += params[i] + " ";
		}
		newTopic = Utils::trimString(newTopic);
		channel.setTopic(newTopic);
		broadcastToChannel(channelName, ":" + getClientNickname(client) + " TOPIC " + channelName + " :" + newTopic + "\r\n"); // Send TOPIC to all in channel
	}
}
