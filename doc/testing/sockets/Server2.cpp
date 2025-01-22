/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Server2.cpp										:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: bmetehri <bmetehri@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/01/22 14:21:12 by bmetehri		  #+#	#+#			 */
/*   Updated: 2025/01/22 14:21:27 by bmetehri		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 6667
#define MAX_CLIENTS 10

void setNonBlocking(int socket) {
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return;
	}
	fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

int main() {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "Error creating socket.\n";
		return 1;
	}

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cerr << "Error binding socket.\n";
		close(serverSocket);
		return 1;
	}

	if (listen(serverSocket, MAX_CLIENTS) == -1) {
		std::cerr << "Error listening on socket.\n";
		close(serverSocket);
		return 1;
	}

	std::cout << "Server is running on port " << PORT << ". Waiting for clients...\n";

	std::vector<int> clients;
	fd_set readFds;
	char buffer[1024];

	setNonBlocking(serverSocket);

	while (true) {
		FD_ZERO(&readFds);
		FD_SET(serverSocket, &readFds);
		int maxFd = serverSocket;

		// Add client sockets to the set
		for (size_t i = 0; i < clients.size(); ++i) {
			FD_SET(clients[i], &readFds);
			if (clients[i] > maxFd) {
				maxFd = clients[i];
			}
		}

		// Wait for activity
		if (select(maxFd + 1, &readFds, NULL, NULL, NULL) < 0) {
			std::cerr << "Error with select().\n";
			break;
		}

		// Check for new client connections
		if (FD_ISSET(serverSocket, &readFds)) {
			sockaddr_in clientAddr;
			socklen_t clientSize = sizeof(clientAddr);
			int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

			if (clientSocket != -1) {
				setNonBlocking(clientSocket);
				clients.push_back(clientSocket);
				std::cout << "New client connected.\n";
			}
		}

		// Check for messages from clients
		for (size_t i = 0; i < clients.size(); ++i) {
			int clientSocket = clients[i];
			if (FD_ISSET(clientSocket, &readFds)) {
				memset(buffer, 0, sizeof(buffer));
				int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

				if (bytesReceived <= 0) {
					std::cout << "Client disconnected.\n";
					close(clientSocket);
					clients.erase(clients.begin() + i);
					--i;
				} else {
					std::cout << "Client: " << buffer;
					send(clientSocket, buffer, bytesReceived, 0);
				}
			}
		}
	}

	// Cleanup
	for (size_t i = 0; i < clients.size(); ++i) {
		close(clients[i]);
	}
	close(serverSocket);

	return 0;
}
