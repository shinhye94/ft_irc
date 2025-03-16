/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePASS.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:04:33 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/16 05:39:52 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void 	Server::handlePASS(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("PASS", "Handling PASS command");
	if (params.empty()) {
		sendToClient(client, ":server 461 * PASS :Not enough parameters\n"); //ERR_NEEDMOREPARAMS
		Debug::commandHandling("PASS", "Error: Not enough parameters");
		return;
	}
	if (authenticateClient(params[0])) {
		client->setAuthenticated(true); // Mark client as authenticated after successful PASS
		sendToClient(client, ":server NOTICE AUTH :*** Password accepted.\n");
		Debug::commandHandling("PASS", "Password accepted - Client Authenticated");
	} else {
		sendToClient(client, ":server 464 * :Password incorrect\n"); //ERR_PASSWDMISMATCH
		Debug::commandHandling("PASS", "Password incorrect - Disconnecting client");
		// removeClient(client); // Disconnect client on wrong password
		_removeTrigger = true;
	}
}
