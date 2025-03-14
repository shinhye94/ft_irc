/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleCommand.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 10:02:21 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/14 08:40:46 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"


void	Server::processCommand(Client* client, const std::string& commandLine) {

	std::vector<std::string> parts = Utils::splitString(commandLine, " ");
	if (parts.empty()) return;

	std::string command = parts[0];
	size_t last_valid = command.find_last_not_of("\r\n");
	if (last_valid != std::string::npos)
		command = command.substr(0, last_valid + 1);
	else
		command.clear(); // command was all \r or \n
	std::vector<std::string>			params;
	std::vector<std::string>::iterator	iter;
	for (iter = params.begin(); iter != params.end(); iter++) {
		size_t last_valid2 = (*iter).find_last_not_of("\r\n");
		if (last_valid2 != std::string::npos)
			(*iter) = (*iter).substr(0, last_valid2 + 1);
		else
			(*iter).clear(); // (*iter) was all \r or \n
	}
	if (parts.size() > 1) {
		params.assign(parts.begin() + 1, parts.end());
	}

	Debug::commandHandling(commandLine, "Processing Command");

	if (command == "PASS") {
		handlePASS(client, params);
	// } else if (!isClientAuthenticated(client)) {
	// 	sendToClient(client, ":server 451 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " :You have not registered\n"); // ERR_NOTREGISTERED
	// 	return; // Ignore further commands until authenticated.
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
		sendToClient(client, ":server 421 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " " + command + " :Unknown command\n"); //ERR_UNKNOWNCOMMAND
		Debug::commandHandling(command, "Unknown command");
	}
}

void 	Server::handlePASS(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("PASS", "Handling PASS command");
	if (params.empty()) {
		sendToClient(client, ":server 461 * PASS :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("PASS", "Error: Not enough parameters");
		return;
	}
	if (authenticateClient(client, params[0])) {
		client->setAuthenticated(true); // Mark client as authenticated after successful PASS
		sendToClient(client, ":server NOTICE AUTH :*** Password accepted.\n");
		Debug::commandHandling("PASS", "Password accepted - Client Authenticated");
	} else {
		sendToClient(client, ":server 464 * :Password incorrect\n"); //ERR_PASSWDMISMATCH
		Debug::commandHandling("PASS", "Password incorrect - Disconnecting client");
		removeClient(client); // Disconnect client on wrong password
	}
}

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

void	Server::handleUSER(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("USER", "Handling USER command");
	if (params.size() < 4) {
		sendToClient(client, ":server 461 * USER :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("USER", "Error: Not enough parameters");
		return;
	}
	std::string username = params[0];
	std::string realname = params[3]; // Realname is usually the 4th param and onwards, but we'll just take the 4th for simplicity here.

	client->setUsername(username);

	std::string nickname = getClientNickname(client);
	if (!nickname.empty()) { // Check if NICK was set before, now complete registration if USER is set.
		sendToClient(client, ":server 001 " + nickname + " :Welcome to the IRC server!\n"); //RPL_WELCOME
		sendToClient(client, ":server 002 " + nickname + " :Your host is server\n"); //RPL_YOURHOST
		sendToClient(client, ":server 003 " + nickname + " :This server was created now\n"); //RPL_CREATED
		sendToClient(client, ":server 004 " + nickname + " server - -\n"); //RPL_MYINFO (minimal)
		Debug::clientEvent("Registration completed after USER", client);
	} else {
		 Debug::clientEvent("Username set", client);
	}
}

void	Server::handleJOIN(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("JOIN", "Handling JOIN command");
	if (params.empty()) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " JOIN :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("JOIN", "Error: Not enough parameters");
		return;
	}
	std::string channelName = params[0];
	if (channelName[0] != '#') {
		sendToClient(client, ":server 476 " + channelName + " :Invalid channel name\n"); // ERR_BADCHANMASK (Simplified error)
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
		Debug::channelEvent("Channel created", channelPtr);
	}

	Channel& channel = *channelPtr;

	if (channel.isInviteOnly() && !channel.isInvited(getClientNickname(client))) {
		sendToClient(client, ":server 473 " + getClientNickname(client) + " " + channelName + " :Cannot join channel (+i)\n"); // ERR_INVITEONLYCHAN
		Debug::commandHandling("JOIN", "Error: Invite-only channel, client not invited: " + channelName);
		return;
	}

	if (channel.hasKey() && !channel.checkKey(key)) {
		sendToClient(client, ":server 475 " + getClientNickname(client) + " " + channelName + " :Cannot join channel (+k)\n"); // ERR_BADCHANNELKEY
		Debug::commandHandling("JOIN", "Error: Incorrect channel key: " + channelName);
		return;
	}

	if (!channel.addUser(client)) { //addUser returns false if limit reached or user already in channel
		 if (channel.isLimitReached()) {
			sendToClient(client, ":server 471 " + getClientNickname(client) + " " + channelName + " :Channel is full (+l)\n"); // ERR_CHANNELISFULL
			Debug::commandHandling("JOIN", "Error: Channel is full: " + channelName);
		 } else if (channel.hasUser(client)) { // Already in channel (should not normally happen from client's perspective in a normal scenario, but handle it).
			 return; // Already in channel, nothing to do.
		 } else { // General error, could extend error codes for more specific reasons if needed.
			 sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\n"); // Or more general error code. ERR_NOSUCHCHANNEL is used as fallback.
			 Debug::commandHandling("JOIN", "Error: Failed to join channel (unknown reason): " + channelName);
			 return;
		 }
	}
	channel.clearInvites(); // Clear invite status upon successful join.
	Debug::channelEvent("Client joined channel", &channel, client);


	std::string nickname = getClientNickname(client);

	// Send JOIN message to the client who joined
	sendToClient(client, ":" + nickname + " JOIN " + channelName + "\n");

	// Send topic (no topic implemented in this example, send RPL_NOTOPIC)
	sendToClient(client, ":server 331 " + nickname + " " + channelName + " :No topic is set\n"); //RPL_NOTOPIC

	// Send NAMES list
	std::string namesList = ":server 353 " + nickname + " = " + channelName + " :"; //RPL_NAMREPLY
	for (std::set<Client*>::iterator user_it = channel.getUsers().begin(); user_it != channel.getUsers().end(); ++user_it) { // Iterator-based loop
		Client* userInChannel = *user_it;
		namesList += getClientNickname(userInChannel) + " ";
	}
	namesList += "\n";
	sendToClient(client, namesList);
	sendToClient(client, ":server 366 " + nickname + " " + channelName + " :End of /NAMES list\n"); //RPL_ENDOFNAMES

	// Notify other channel members about the join
	broadcastToChannel(channelName, ":" + nickname + " JOIN " + channelName + "\n", client);
	Debug::channelEvent("JOIN command completed", &channel, client);
}


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

	broadcastToChannel(channelName, ":" + nickname + " PART " + channelName + "\n", client);
	sendToClient(client, ":" + nickname + " PART " + channelName + "\n");
	Debug::channelEvent("Client parted channel", &channel, client);

	if (channel.getUserCount() == 0) {
		_channels.erase(channelName); // Remove channel if empty
		Debug::channelEvent("Channel removed as it became empty", &channel);
	}
	Debug::channelEvent("PART command completed", &channel, client);
}

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

void	Server::handleQUIT(Client* client) {
	Debug::commandHandling("QUIT", "Handling QUIT command");
	sendToClient(client, ":server NOTICE :Closing connection\n");
	// removeClient(client);
	_removeTrigger = true;
	Debug::commandHandling("QUIT command completed", "");
}

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

void	Server::handleWHO(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("WHO", "Handling WHO command");
	std::string target = "*"; // Default target is all users
	if (!params.empty()) {
		target = params[0]; // Could be a channel or a mask. (Simplified to just channel or all for now).
	}

	std::string nickname = getClientNickname(client);

	if (target[0] == '#') { // WHO for a channel.
		Channel* channelPtr = findChannel(target);
		if (channelPtr) {
			for (std::set<Client*>::iterator channel_client_it = channelPtr->getUsers().begin(); channel_client_it != channelPtr->getUsers().end(); ++channel_client_it) { // Iterator-based loop
				Client* channelClient = *channel_client_it;
				sendToClient(client, ":server 352 " + nickname + " " + target + " " + channelClient->getUsername() + " server " + _serverPassword + " " + getClientNickname(channelClient) + " H :0 " + channelClient->getUsername() + "\n"); //RPL_WHOREPLY (simplified)
			}
			Debug::channelEvent("WHO list sent for channel", channelPtr, client);
		} else {
			Debug::commandHandling("WHO", "No such channel for WHO: " + target);
		}
	} else { // WHO for all users. (Simplified - no mask support for now)
		for (std::set<Client*>::iterator list_client_it = _clients.begin(); list_client_it != _clients.end(); ++list_client_it) { // Iterator-based loop
			Client* listClient = *list_client_it;
			 sendToClient(client, ":server 352 " + nickname + " * " + listClient->getUsername() + " server " + _serverPassword + " " + getClientNickname(listClient) + " H :0 " + listClient->getUsername() + "\n"); //RPL_WHOREPLY (simplified)
		}
		Debug::commandHandling("WHO", "WHO list sent for all users");
	}
	sendToClient(client, ":server 315 " + nickname + " " + target + " :End of WHO list\n"); //RPL_ENDOFWHO
	Debug::commandHandling("WHO command completed", "Target: " + target);
}

void	Server::handlePING(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("PING", "Handling PING command");
	std::string pingMessage = "server"; // Default PING message if none provided.
	if (!params.empty()) {
		pingMessage = params[0]; // Take the first parameter as ping message.
	}
	sendToClient(client, ":server PONG " + pingMessage + "\n");
	Debug::commandHandling("PING command completed", "PONG sent with message: " + pingMessage);
}

void	Server::handleKICK(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("KICK", "Handling KICK command");
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " KICK :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
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
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\n"); // ERR_NOSUCHCHANNEL
		Debug::commandHandling("KICK", "Error: No such channel: " + channelName);
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.isOperator(client)) {
		sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\n"); // ERR_CHANOPRIVSNEEDED
		Debug::commandHandling("KICK", "Error: Not channel operator for: " + channelName);
		return;
	}

	Client* targetClient = findClientByNickname(kickTargetNickname);
	if (!targetClient || !channel.hasUser(targetClient)) {
		sendToClient(client, ":server 441 " + getClientNickname(client) + " " + kickTargetNickname + " " + channelName + " :They aren't on that channel\n"); // ERR_USERNOTINCHANNEL
		Debug::commandHandling("KICK", "Error: Target user not in channel: " + kickTargetNickname + ", Channel: " + channelName);
		return;
	}

	broadcastToChannel(channelName, ":" + getClientNickname(client) + " KICK " + channelName + " " + kickTargetNickname + " :" + kickReason + "\n");
	sendToClient(targetClient, ":" + getClientNickname(client) + " KICK " + channelName + " " + kickTargetNickname + " :" + kickReason + "\n"); // Also send to the kicked user
	Debug::channelEvent("Client kicked from channel", &channel, client);

	channel.removeUser(targetClient);
	if (channel.getUserCount() == 0) {
		_channels.erase(channelName); // Remove channel if empty
		Debug::channelEvent("Channel removed as it became empty after kick", &channel);
	}
	Debug::commandHandling("KICK command completed", "Target: " + kickTargetNickname + ", Channel: " + channelName);
}

void	Server::handleINVITE(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("INVITE", "Handling INVITE command");
	if (params.size() < 2) {
		sendToClient(client, ":server 461 " + getClientNickname(client) + " INVITE :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("INVITE", "Error: Not enough parameters");
		return;
	}
	std::string inviteeNickname = params[0];
	std::string channelName = params[1];

	Channel* channelPtr = findChannel(channelName);
	if (!channelPtr) {
		sendToClient(client, ":server 403 " + getClientNickname(client) + " " + channelName + " :No such channel\n"); // ERR_NOSUCHCHANNEL
		Debug::commandHandling("INVITE", "Error: No such channel: " + channelName);
		return;
	}
	Channel& channel = *channelPtr;

	if (!channel.hasUser(client)) {
		sendToClient(client, ":server 442 " + getClientNickname(client) + " " + channelName + " :You're not on that channel\n"); // ERR_NOTONCHANNEL
		Debug::commandHandling("INVITE", "Error: Not on channel: " + channelName);
		return;
	}

	if (channel.isInviteOnly() && !channel.isOperator(client)) {
		sendToClient(client, ":server 482 " + getClientNickname(client) + " " + channelName + " :You're not channel operator\n"); // ERR_CHANOPRIVSNEEDED
		Debug::commandHandling("INVITE", "Error: Not channel operator for invite-only channel: " + channelName);
		return;
	}

	Client* inviteeClient = findClientByNickname(inviteeNickname);
	if (!inviteeClient) {
		sendToClient(client, ":server 401 " + getClientNickname(client) + " " + inviteeNickname + " :No such nick/channel\n"); // ERR_NOSUCHNICK
		Debug::commandHandling("INVITE", "Error: No such nickname to invite: " + inviteeNickname);
		return;
	}

	if (channel.hasUser(inviteeClient)) {
		sendToClient(client, ":server 443 " + getClientNickname(client) + " " + inviteeNickname + " " + channelName + " :is already on channel\n"); // ERR_USERONCHANNEL
		Debug::commandHandling("INVITE", "Error: User already on channel: " + inviteeNickname + ", Channel: " + channelName);
		return;
	}

	channel.inviteUser(inviteeNickname); // Add to invite list.
	sendToClient(client, ":server 341 " + getClientNickname(client) + " " + inviteeNickname + " " + channelName + "\n"); //RPL_INVITING
	sendToClient(inviteeClient, ":" + getClientNickname(client) + " INVITE " + inviteeNickname + " :" + channelName + "\n"); //Send INVITE notice to invited user.
	Debug::channelEvent("Client invited to channel", &channel, inviteeClient);
	Debug::clientEvent("Client invited by", client);
	Debug::commandHandling("INVITE command completed", "Invitee: " + inviteeNickname + ", Channel: " + channelName);
}


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
		broadcastToChannel(channelName, ":" + getClientNickname(client) + " TOPIC " + channelName + " :" + newTopic + "\n"); // Send TOPIC to all in channel
		Debug::channelEvent("Topic changed", &channel, client);
		Debug::commandHandling("TOPIC", "Topic changed to: " + newTopic);
	}
	Debug::commandHandling("TOPIC command completed", "Channel: " + channelName);
}
