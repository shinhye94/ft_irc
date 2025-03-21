/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:22:39 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 03:59:19 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/Debug.hpp"


		// Static pointer to the IRCServer instance for signal handling
		Server* instance = NULL;
		// Static flag to indicate if shutdown was requested
		volatile bool shutdownRequested = false;

		// Static signal handler function (C++98 compatible)
		void signalHandler(int signum) {
			if (signum == SIGINT) {
			std::cout << "\nSIGINT received. Shutting down server..." << std::endl;
			shutdownRequested = true;
			}
		}

		Server::Server( int port, const std::string& password ) : _portNumber(port), _serverPassword(password), _currentCommand(""), _removeTrigger(false) {
			Debug::serverPhase("FT_IRC_Server instance created.");
			instance = this;
			// Register the signal handler for SIGINT (Ctrl+C)
			if (signal(SIGINT, signalHandler) == SIG_ERR) {
				perror("signal");
				exit(EXIT_FAILURE);
			}
		};

		Server::~Server( void ) {
			Debug::serverPhase("FT_IRC_Server instance destroyed.");
			if (_serverSocket != -1) {
				close(_serverSocket);
				Debug::serverPhase("Server socket closed.");
			}
			std::set<Client*>::iterator	it;
			for (it = _clients.begin(); it != _clients.end(); it++) {
				Client* currentClient = *it;
				currentClient->disconnect();
				delete currentClient;
			}
			Debug::serverPhase("All clients disconnected and resources cleaned.");
		}

void	Server::run( void ) {
	setupServerSocket();
	Debug::serverPhase("Entering main server loop.");

	_fdMax = _serverSocket;

	time_t last_table_print_time; // Changed to time_t
	time(&last_table_print_time); // Initialize last_table_print_time

	while (!shutdownRequested) {
		FD_ZERO(&_readfds);
		FD_SET(_serverSocket, &_readfds);
		FD_SET(STDIN_FILENO, &_readfds); // add stdin to watch for server commands

		_fdMax = _serverSocket;
		std::set<Client*>::iterator	it;
		for (it = _clients.begin(); it != _clients.end(); it++){
			Client* currentClient = *it;
			if (currentClient) { //tmp
				FD_SET(currentClient->getSocketFD(), &_readfds);
				if (currentClient->getSocketFD() > _fdMax) {
					_fdMax = currentClient->getSocketFD();
				}
			} //tmp
		}

		if (select(_fdMax + 1, &_readfds, 0, 0, 0) < 0 && !shutdownRequested) {
			perror("Error: server: Couldn't (select) socket");
			continue;
		}

		if (shutdownRequested) {
			break; // Exit the loop if shutdown was requested during select
		}

		if (FD_ISSET(_serverSocket, &_readfds)) {
			acceptNewClient();
		}

		if (FD_ISSET(STDIN_FILENO, &_readfds)) {
			std::string	serverCommand;
			std::getline(std::cin, serverCommand);
			if (serverCommand == "shutdown") {
				Debug::serverPhase("Shutdown command received from console.");
				std::cout << "Server shutting down..." << std::endl;
				shutdownRequested = true;
				break;
			} else {
				std::cout << "Unknown server command: " << serverCommand << std::endl;
			}
		}

		std::set<Client*>				clientsToRemove;
		std::set<Client*>::iterator		iter;
		for (iter = _clients.begin(); iter != _clients.end(); iter++) {
			Client* currentClient = *iter;
			if (currentClient) { //tmp
				if (FD_ISSET(currentClient->getSocketFD(), &_readfds)) {
					handleClientData(currentClient);
					if ((FD_ISSET(currentClient->getSocketFD(), &_readfds) && errno == ECONNRESET) || currentClient->isToBeRemoved()) {
						clientsToRemove.insert(currentClient);
						_removeTrigger = false;
					}
				}
			} //tmp
		}

		std::set<Client*>::iterator	iterr;
		for (iterr = clientsToRemove.begin(); iterr != clientsToRemove.end(); iterr++) {
			Client* currentClient = *iterr;
			removeClient(currentClient);
		}

		std::map<std::string, Channel>::iterator iterCH = _channels.begin();
		std::map<std::string, Channel>::iterator tmp;
		// for (iterCH = _channels.begin(); iterCH != _channels.end(); iterCH++) {
		// 	tmp = iterCH;
		// 	tmp++;
		// 	Channel&	channel = iterCH->second;
		// 	if (channel.isToBeRemoved())
		// 		_channels.erase(channel.getName());
		// }
		while (iterCH != _channels.end()) {
			tmp = iterCH;
			tmp++;
			Channel&	channel = iterCH->second;
			if (channel.isToBeRemoved()) {
				_channels.erase(channel.getName());
				iterCH = tmp;
				continue;
			}
			iterCH++;
		}

		// Periodically print client table (every 2 seconds for example)
		time_t now;
		time(&now); // Get current time
		if (difftime(now, last_table_print_time) >= 2) { // Compare time difference in seconds
			Debug::printClientTable(_clients);
			time(&last_table_print_time); // Update last print time
		}
		usleep(50 * 1000); // Small sleep to reduce CPU usage (usleep in microseconds) - C++98 compatible replacement for std::this_thread::sleep_for
	}
	Debug::serverPhase("Exiting main server loop.");
}
