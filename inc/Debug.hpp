/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 09:49:38 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/10 11:38:39 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Colors.hpp"
#include "Client.hpp"

namespace Debug {

// Debug Levels
enum DebugLevel {
	DEBUG_LEVEL_NONE    = 0,
	DEBUG_LEVEL_INFO    = 1 << 0, // General info messages (startup, shutdown, connections)
	DEBUG_LEVEL_COMMAND = 1 << 1, // Command received and processed
	DEBUG_LEVEL_TABLE   = 1 << 2, // Client table updates
	DEBUG_LEVEL_CHANNEL = 1 << 3, // Channel related events (join, part, mode changes etc.)
	DEBUG_LEVEL_ALL     = DEBUG_LEVEL_INFO | DEBUG_LEVEL_COMMAND | DEBUG_LEVEL_TABLE | DEBUG_LEVEL_CHANNEL
};

extern DebugLevel currentDebugLevel;

// Global Debug Level Setting Function (Can be set from main or command line arg later)
void setDebugLevel(DebugLevel level);

// --- General Debug Functions ---
void serverPhase(const std::string& phase);
void clientEvent(const std::string& event, const Client* client);
void commandHandling(const std::string& command, const std::string& handler);
void channelEvent(const std::string& event, const Channel* channel, const Client* client = NULL);

// --- Client Table Debug ---
void printClientTable(const std::set<Client*>& clients);

// --- Command Debug ---
void printCommand(const std::string& command, const Client* client);

std::string intToString(int n);

// --- Utility Functions ---
void clearScreen();
std::string padRight(const std::string& str, int width); // New padding function declaration

} // namespace Debug
