/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleNICK.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 21:05:48 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 09:29:38 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"

void	Server::handleNICK(Client* client, const std::vector<std::string>& params) {
	Debug::commandHandling("NICK", "Handling NICK command");
	if (params.empty()) {
		sendToClient(client, ":server 431 * :No nickname given\r\n"); //ERR_NONICKNAMEGIVEN
		Debug::commandHandling("NICK", "Error: No nickname given");
		return;
	}
	std::string nickname = params[0];

	// for (size_t i = 0; i < nickname.length(); ++i) {
	// 	int c = nickname[i];
	// 	std::cout << "c: " << c << std::endl;
	// }

	// Rule 1: No leading # character or other character advertised in CHANTYPES
	const std::string chantypes = "#&"; // For this exercise, assume only '#'
	if (!nickname.empty() && chantypes.find(nickname[0]) != std::string::npos) {
		sendToClient(client, ":server 432 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " " + nickname + " :Erroneous nickname\r\n"); //ERR_ERRONEUSNICKNAME
		Debug::commandHandling("NICK", "Error: Nickname starts with a channel type character: " + nickname);
		return;
	}

	// Rule 2: No leading colon (:)
	if (!nickname.empty() && nickname[0] == ':') {
		sendToClient(client, ":server 432 " + (getClientNickname(client).empty() ? "*" : getClientNickname(client)) + " " + nickname + " :Erroneous nickname\r\n"); //ERR_ERRONEUSNICKNAME
		Debug::commandHandling("NICK", "Error: Nickname starts with a colon: " + nickname);
		return;
	}

	// Rule 3: No ASCII space
    // 닉네임 길이 제한 (기본은 9자, 필요하면 확장 가능)
    if (nickname.empty() || nickname.length() > 9)
        return ;

    // 허용되는 첫 글자: 알파벳 또는 특정 특수문자
    std::string validSpecialStart = "[]\\`_^{|}";
    if (!(std::isalpha(nickname[0]) || validSpecialStart.find(nickname[0]) != std::string::npos))
        return ;

    // 허용되는 나머지 문자: 알파벳, 숫자, 특정 특수문자 (-[]\`_^{|})
    std::string validChars = "-[]\\`_^{|}";
    for (std::string::size_type i = 1; i < nickname.length(); i++) {
        if (!(std::isalnum(nickname[i]) || validChars.find(nickname[i]) != std::string::npos))
            return ;
    }

	// nickname = nickname.substr(0, nickname.length() - 1);
	if (isNicknameInUse(nickname)) {
		sendToClient(client, ":server 433 * " + nickname + " :Nickname is already in use\r\n"); //ERR_NICKNAMEINUSE
		Debug::commandHandling("NICK", "Error: Nickname already in use: " + nickname);
		return;
	}

	std::string oldNickname = getClientNickname(client);
	client->setNickname(nickname);

	std::string username = client->getUsername();
	std::string hostname = client->getHostname();
	std::string servername = client->getServername();

	if (oldNickname.empty()) { // First NICK command after connection
		sendToClient(client, ":server NOTICE AUTH :*** Your nickname is now " + nickname + "\r\n");
		Debug::clientEvent("Nickname set", client);
	} else { // Nickname change
		for (std::map<std::string, Channel>::iterator chan_it = _channels.begin(); chan_it != _channels.end(); ++chan_it) { // Iterator-based loop
			Channel& channel = chan_it->second;
			if (channel.hasUser(client)) {
				broadcastToChannel(chan_it->first, ":" + oldNickname + "!" + username + "@" + hostname + " NICK "+ chan_it->first + " :" + _currentCommand + "\r\n", client);
				Debug::channelEvent("NICK change broadcasted to channel", &channel, client);
			}
		}
		sendToClient(client, ":" + oldNickname + "!" + username + "@" + hostname + " NICK :"+ nickname + "\r\n"); // Notify the user of their own nick change
		Debug::clientEvent("Nickname changed", client);
	}

	if (!client->getUsername().empty() && oldNickname.empty()) { // Check if USER was set before, now complete registration if NICK is set for the first time.
		sendToClient(client, ":server 001 " + nickname + " :Welcome to the IRC server!\r\n"); //RPL_WELCOME
		sendToClient(client, ":server 002 " + nickname + " :Your host is " + servername + "\r\n"); //RPL_YOURHOST
		sendToClient(client, ":server 003 " + nickname + " :This server was created now\r\n"); //RPL_CREATED
		sendToClient(client, ":server 004 " + nickname + " " + servername + "\r\n"); //RPL_MYINFO (minimal)
		Debug::clientEvent("Registration completed", client);
	}
}
