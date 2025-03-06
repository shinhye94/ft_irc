/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/27 10:01:58 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 12:53:52 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

void	Server::setupServerSocket( void ) {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1){
		perror("Error: server: Couldn't create server socket\n");
		exit(1);
	}

	int		option = REUSEADDR_OPTION;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
		perror("Error: server: couldn't configure SO_REUSEADDR.\n");
		exit(1);
	}

	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0) {
		perror("Error: server: couldn't configure socket in non block.\n");
		exit(1);
	}

	sockaddr_in		serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(_portNumber);

	if (bind(_serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		perror("Error: server: Couldn't bind server socket\n");
		close(_serverSocket);
		exit(1);
	}

	if (listen(_serverSocket, MAX_CLIENTS) < 0) {
		perror("Error: server: Couldn't listen for other sockets");
		close(_serverSocket);
		exit(1);
	}

	std::cout << "ft_irc Server listening on port: " << _portNumber << std::endl;
}

void	Server::acceptNewClient( void ) {
	sockaddr_in		clientAddress;
	socklen_t		clientAddressLength = sizeof(clientAddress);
	int				newClientSocketFD = accept(_serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
	if (newClientSocketFD < 0) {
		perror("Error: server: Couldn't accept client\n");
		return ;
	}

	Client*	newClient = new Client(newClientSocketFD);
	_clients.insert(newClient);
	_clientMap[newClientSocketFD] = newClient;

	sendToClient(newClient, ":server NOTICE AUTH :*** Welcome to the IRC server.\r\n");
	sendToClient(newClient, ":server NOTICE AUTH :*** Please enter password with PASS <password>\r\n");

	std::cout << "New client connected from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;
}

void	Server::handleClientData(Client* client) {
	char	buffer[512];
	int		bytesReceived = client->receiveData(buffer, sizeof(buffer));

	if (bytesReceived > 0) {
		std::string			receivedData(buffer);
		std::stringstream	ss(receivedData);
		std::string			line;
		while (std::getline(ss, line)) {
			processCommand(client, Utils::trimString(line));
		}
	} else if (bytesReceived == 0) {
		std::cout << "Client disconnectd: " << client->getSocketFD() << std::endl;
		removeClient(client);
	} else {
		perror("Error: receiveData Internal error\n");
		removeClient(client);
	}
}

void	Server::removeClient(Client* client) {
	if (!client) return;

	int clientSocketFD = client->getSocketFD();
	std::string		nickname = getClientNickname(client);
	std::cout << "Removing client " << clientSocketFD << (nickname.empty() ? " " : " Nickname: " + nickname + " )") << std::endl;
	std::map<std::string, Channel>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++) {
		const std::string& key = it->first;
		Channel& channel = it->second;
		if (channel.hasUser(client)) {
			channel.removeUser(client);
			if (!nickname.empty()) {
				if (channel.getUserCount() > 0) {
					broadcastToChannel(key, ":" + nickname + " PART " + key + "\r\n", client);
				}
			}
		}
	}
	_clients.erase(client);
	_clientMap.erase(clientSocketFD);
	delete client;
}


Client* Server::findClientBySocket(int socket_fd) {
	std::map<int, Client*>::iterator it = _clientMap.find(socket_fd);
	return (it != _clientMap.end()) ? it->second : 0;
}


Client* Server::findClientByNickname(const std::string& nickname) {
	for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if ((*it)->getNickname() == nickname) {
			return *it;
		}
	}
	return 0;
}


Channel* Server::findChannel(const std::string& channelName) {
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	return (it != _channels.end()) ? &it->second : 0;
}
