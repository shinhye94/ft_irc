/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 01:49:13 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 01:49:59 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Utils.hpp"
#include "Client.hpp"
#include <iostream>
#include <sstream>

Client::Client(const std::string& serverAddress, int port, const std::string& password)
	: _serverAddressStr(serverAddress), _portNumber(port), _clientPassword(password) {}

Client::~Client(void) {
	if (_clientSocket != -1) {
		close(_clientSocket);
	}
}

bool		Client::connectToServer( void ) {
	_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_clientSocket == -1) {
		perror("Error: Couldn't create socket\n");
		return (false);
	}

	sockaddr_in	serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_portNumber);
	if (inet_pton(AF_INET, _serverAddressStr.c_str(), &serverAddress.sin_addr) <= 0) {
		perror("Error: inet_pton throwed an error\n");
		close(_clientSocket);
		_clientSocket = -1;
		return (false);
	}

	if (connect(_clientSocket, (struct sockaddr*)_serverAddressStr.c_str(), sizeof(serverAddress)) <= 0) {
		perror("Error: Couldn't connect to the server\n");
		close(_clientSocket);
		_clientSocket = -1;
		return (false);
	}

	std::cout << "Connected to server " << _serverAddressStr << ":" << _portNumber << std::endl;
	return (true);
}

void	Client::run( void ) {
	if (!connectToServer()) {
		return ;
	}

	sendCommand("PASS " + _clientPassword);

	// std::thread responseThread(&Client::handleServerResponse, this);
	handleServerResponse();
	handleUserInput();
	// responseThread.join();
}

void	Client::handleServerResponse() {
	char	buffer[512];

	while (true)
	{
		int	bytesReceived = Utils::receiveData(_clientSocket, buffer, sizeof(buffer));
		if (bytesReceived > 0) {
			std::cout << buffer;
		} else if (bytesReceived == 0) {
			std::cout << "Server disconnected." << std::endl;
			break ;
		} else {
			perror("Error: client couldn't catch ServerData anymore\n");
			break ;
		}
	}
}

void	Client::handleUserInput() {
	std::string	userInput;
	while (true)
	{
		std::getline(std::cin, userInput);
		if (userInput == "quit") {
			sendCommand("QUIT :Client Quit");
			break ;
		}
		sendCommand(userInput);
	}
	close(_clientSocket);
}

void	Client::sendCommand(const std::string& command) {
	if (_clientSocket != -1) {
		if (Utils::sendData(_clientSocket, command + "\r\n") != 0) {
			std::cerr << "Error sending command to server." << std::endl;
			close(_clientSocket);
			_clientSocket = -1;
		}
	} else {
		std::cerr << "Not connected to server." << std::endl;
	}
}
