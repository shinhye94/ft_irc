/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 09:36:43 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/14 00:46:12 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../inc/Debug.hpp"
#include <sstream>

namespace Debug {

	DebugLevel currentDebugLevel = DEBUG_LEVEL_ALL; // Default debug level

	std::string intToString(int n) {
		std::ostringstream stream;
		stream << n;
		return stream.str();
	}

	void setDebugLevel(DebugLevel level) {
		currentDebugLevel = level;
		if (currentDebugLevel != DEBUG_LEVEL_NONE) {
			serverPhase("Debug level set to: " + intToString(level)); // Use intToString
		}
	}


	// --- Utility Functions ---
	void clearScreen() {
		std::cout << "\033[2J\033[H"; // ANSI escape codes to clear screen and reset cursor
	}

	// New function to pad string to the right with spaces
	std::string padRight(const std::string& str, int width) {
		if ((int)str.length() >= width) {
			return str;
		}
		std::string paddedString = str;
		paddedString.append(width - str.length(), ' ');
		return paddedString;
	}

	// Helper function to convert int to string (C++98 compatible)


	// --- General Debug Functions ---
	void serverPhase(const std::string& phase) {
		if (currentDebugLevel & DEBUG_LEVEL_INFO) {
			Colors::printColored(CYAN, "[SERVER PHASE] ", false); // Use CYAN directly
			Colors::printColored(WHITE, phase, true);        // Use WHITE directly
		}
	}

	void clientEvent(const std::string& event, const Client* client) {
		if (currentDebugLevel & DEBUG_LEVEL_INFO) {
			Colors::printColored(BLUE, "[CLIENT EVENT] ", false); // Use BLUE directly
			Colors::printColored(WHITE, event + " - FD: " + intToString(client->getSocketFD()) + ", Nick: " + client->getNickname(), true); // Use intToString and WHITE
		}
	}

	void commandHandling(const std::string& command, const std::string& handler) {
		if (currentDebugLevel & DEBUG_LEVEL_COMMAND) {
			Colors::printColored(YELLOW, "[COMMAND HANDLER] ", false); // Use YELLOW directly
			Colors::printColored(WHITE, "Command: " + command + ", Handler: " + handler, true); // Use WHITE directly
		}
	}

	void channelEvent(const std::string& event, const Channel* channel, const Client* client) {
		if (currentDebugLevel & DEBUG_LEVEL_CHANNEL) {
			Colors::printColored(MAGENTA, "[CHANNEL EVENT] ", false); // Use MAGENTA directly
			std::string clientInfo = client ? "Client Nick: " + client->getNickname() + ", FD: " + intToString(client->getSocketFD()) : "N/A"; // Use intToString
			Colors::printColored(WHITE, event + " - Channel: " + channel->getName() + ", " + clientInfo, true); // Use WHITE directly
		}
	}


	// --- Client Table Debug ---
	void printClientTable(const std::set<Client*>& clients) {
		if (!(currentDebugLevel & DEBUG_LEVEL_TABLE)) return;

		// clearScreen(); // Clear the console for table refresh

		Colors::printColored(YELLOW, "--- Connected Clients Table ---", true); // Use YELLOW directly
		std::cout << CYAN << padRight("FD", 10) << padRight("Nickname", 20) << padRight("Username", 20) << padRight("Authenticated", 15) << RESET << std::endl; // Use CYAN and RESET directly
		std::cout << CYAN << std::string(65, '-') << RESET << std::endl; // Use CYAN and RESET directly

		if (clients.empty()) {
			Colors::printColored(WHITE, "No clients connected.", true); // Use WHITE directly
		} else {
			std::set<Client *>::iterator	iter;
			for (iter = clients.begin(); iter != clients.end(); iter++) {
				Client* client = *iter;
				std::cout << padRight(intToString(client->getSocketFD()), 10) // Use intToString
						  << padRight(client->getNickname(), 20)
						  << padRight(client->getUsername(), 20)
						  << padRight(client->isAuthenticated() ? "Yes" : "No", 15)
						  << std::endl;
			}
		}
		 Colors::printColored(YELLOW, "--- End of Client Table ---", true); // Use YELLOW directly
	}


	// --- Command Debug ---
	void printCommand(const std::string& command, const Client* client) {
		if (currentDebugLevel & DEBUG_LEVEL_COMMAND) {
			Colors::printColored(GREEN, "[COMMAND RECEIVED] ", false); // Use GREEN directly
			Colors::printColored(WHITE, "Command: " + command + ", Client FD: " + intToString(client->getSocketFD()) + ", Nick: " + client->getNickname(), true); // Use intToString and WHITE
		}
	}

	} // namespace Debug
