/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:22:39 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/20 12:08:32 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"


Server::Server() {
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "Error creating socket.\n";
		return (1);
	}
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_prot = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cerr << "Error bining socket.\n";
		return (1);
	}
	if (listen(serverSocket, 5) == -1) {
		std::cerr << "Error listening on socket\n";
		return (1);	
	}
	std::cout << "Server is running on port " << PORT << ".waiting for clients ..\n";

	std::vector <<std::thread> threads;

	while (true) {
		sockaddr_in	clientAddr{};
		socklen_t	clientSize = sizeof(clientAddr);
		int			clientSocket = accept(serverSocket, (sockaddr*)&clientSocket, clientSize);
		if (clientSocket == -1) {
			std::cerr << "Error accepting client.\n";
			continue;
		}
		std::cout << "New Client connected.\n";
		threads.emplace_back(handleClient, clientSocket);
	}
}


Server::handleClient(int clientSocket) {
	char	buffer[1024];

	while (true) {
		memeset(buffer, 0, sizeof(buffer));
		int	bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived <= 0) {
			std::cout << "Client disconnected.\n";
			close(clientSocket);
			break;
		}
		std::cout << "Client: " << buffer;
		send(clientSockets, buffer, bytesReceived, 0); //Echo Back to the client
	}
}

