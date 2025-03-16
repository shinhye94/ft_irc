/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleACommand.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 10:02:21 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/16 05:43:10 by bmetehri         ###   ########.fr       */
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
	} else if (!isClientAuthenticated(client)) {
		sendToClient(client, ":server 451 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " :You have not registered\n"); // ERR_NOTREGISTERED
		// _removeTrigger = true;
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
		sendToClient(client, ":server 421 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " " + command + " :Unknown command\n"); //ERR_UNKNOWNCOMMAND
		Debug::commandHandling(command, "Unknown command");
	}
}
