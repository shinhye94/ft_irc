/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePING.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:10:22 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/15 21:16:34 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handlePING(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("PING", "Handling PING command");
	std::string pingMessage = "server"; // Default PING message if none provided.
	if (!params.empty()) {
		pingMessage = params[0]; // Take the first parameter as ping message.
	}
	sendToClient(client, ":server PONG " + pingMessage + "\n");
	Debug::commandHandling("PING command completed", "PONG sent with message: " + pingMessage);
}
