/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleMessage.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 10:02:43 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 12:36:29 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

void	Server::broadcastToChannel(const std::string& channelName, const std::string& message, Client* sender) {
	Channel* channelPtr = findChannel(channelName);
	if (channelPtr) {
		channelPtr->broadcastMessage(message, sender);
	}
}

void	Server::sendToClient(Client* client, const std::string& message) {
	if (client) { // Check if client pointer is valid before sending.
		 client->sendMessage(message);
	}
}

static void		someFunction(Client* client) {
	(void)client; // Mark as intentionally unused
}
bool	Server::authenticateClient(Client* client, const std::string& passwordAttempt) {
	someFunction(client);
	return (passwordAttempt == _serverPassword); // Simple password check
}

bool	Server::isClientAuthenticated(Client* client) {
	return client && client->isAuthenticated(); // Check if client object is valid and authenticated.
}

bool	Server::isNicknameInUse(const std::string& nickname) {
	std::set<Client *>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); it++) {
		Client*	client = *it;
		if (client->getNickname() == nickname) {
			return true;
		}
	}
	return false;
}

std::string	Server::getClientNickname(Client* client) {
	return (client ? client->getNickname() : "");
}
