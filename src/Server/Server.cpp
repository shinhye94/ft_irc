/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:22:39 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 12:00:37 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

		Server::Server( int port, const std::string& password ) : _portNumber(port), _serverPassword(password) {};

		Server::~Server( void ) {
			if (_serverSocket != -1)
				close(_serverSocket);

			std::set<Client*>::iterator	it;
			for (it = _clients.begin(); it != _clients.end(); it++) {
				Client* currentClient = *it;
				currentClient->disconnect();
				delete currentClient;
			}
		}

void	Server::run( void ) {
	setupServerSocket();

	_fdMax = _serverSocket;

	while (true) {
		FD_ZERO(&_readfds);
		FD_SET(_serverSocket, &_readfds);
		FD_SET(STDIN_FILENO, &_readfds); // add stdin to watch for server commands

		_fdMax = _serverSocket;
		std::set<Client*>::iterator	it;
		for (it = _clients.begin(); it != _clients.end(); it++){
			Client* currentClient = *it;
			FD_SET(currentClient->getSocketFD(), &_readfds);
			if (currentClient->getSocketFD() > _fdMax) {
				_fdMax = currentClient->getSocketFD();
			}
		}

		if (select(_fdMax + 1, &_readfds, 0, 0, 0) < 0) {
			perror("Error: server: Couldn't (select) socket");
			continue;
		}

		if (FD_ISSET(_serverSocket, &_readfds)) {
			acceptNewClient();
		}

		if (FD_ISSET(STDIN_FILENO, &_readfds)) {
			std::string	serverCommand;
			std::getline(std::cin, serverCommand);
			if (serverCommand == "shutdown") {
				std::cout << "Server shutting down..." << std::endl;
				break;
			} else {
				std::cout << "Unknown server command: " << serverCommand << std::endl;
			}
		}

		std::set<Client*>				clientsToRemove;
		std::set<Client*>::iterator		iter;
		for (iter = _clients.begin(); iter != _clients.end(); iter++) {
			Client* currentClient = *iter;
			if (FD_ISSET(currentClient->getSocketFD(), &_readfds)) {
				handleClientData(currentClient);
				if (FD_ISSET(currentClient->getSocketFD(), &_readfds) && errno == ECONNRESET) {
					clientsToRemove.insert(currentClient);
				}
			}
		}

		std::set<Client*>::iterator	iterr;
		for (iterr = clientsToRemove.begin(); iterr != clientsToRemove.end(); iterr++) {
			Client* currentClient = *iterr;
			removeClient(currentClient);
		}
	}
}
