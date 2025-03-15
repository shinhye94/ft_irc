/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleQUIT.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:08:44 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/15 21:15:01 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleQUIT(Client* client) {
	Debug::commandHandling("QUIT", "Handling QUIT command");
	sendToClient(client, ":server NOTICE :Closing connection\n");
	// removeClient(client);
	_removeTrigger = true;
	Debug::commandHandling("QUIT command completed", "");
}
