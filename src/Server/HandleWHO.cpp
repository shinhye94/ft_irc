/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleWHO.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:10:06 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 07:48:34 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

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
				sendToClient(client, ":server 352 " + nickname + " " + target + " " + channelClient->getUsername() + " server " + _serverPassword + " " + getClientNickname(channelClient) + " H :0 " + channelClient->getUsername() + "\r\n"); //RPL_WHOREPLY (simplified)
			}
			Debug::channelEvent("WHO list sent for channel", channelPtr, client);
		} else {
			Debug::commandHandling("WHO", "No such channel for WHO: " + target);
		}
	} else { // WHO for all users. (Simplified - no mask support for now)
		for (std::set<Client*>::iterator list_client_it = _clients.begin(); list_client_it != _clients.end(); ++list_client_it) { // Iterator-based loop
			Client* listClient = *list_client_it;
			 sendToClient(client, ":server 352 " + nickname + " * " + listClient->getUsername() + " server " + _serverPassword + " " + getClientNickname(listClient) + " H :0 " + listClient->getUsername() + "\r\n"); //RPL_WHOREPLY (simplified)
		}
		Debug::commandHandling("WHO", "WHO list sent for all users");
	}
	sendToClient(client, ":server 315 " + nickname + " " + target + " :End of WHO list\r\n"); //RPL_ENDOFWHO
	Debug::commandHandling("WHO command completed", "Target: " + target);
}
