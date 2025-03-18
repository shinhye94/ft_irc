/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleQUIT.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:08:44 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 08:12:34 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleQUIT(Client* client) {
	Debug::commandHandling("QUIT", "Handling QUIT command");
	std::string nickname = getClientNickname(client);
	std::string username = client->getUsername();
	std::string hostname = client->getHostname();
	sendToClient(client, ":" + nickname + "!" + username + "@" + hostname + _currentCommand + "\r\n");
	// removeClient(client);
	// _removeTrigger = true;
	client->setRemoveSituation(true);
	Debug::commandHandling("QUIT command completed", "");
}
