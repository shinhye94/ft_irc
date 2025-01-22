/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Server1.cpp										:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: bmetehri <bmetehri@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/01/22 14:21:08 by bmetehri		  #+#	#+#			 */
/*   Updated: 2025/01/22 14:36:15 by bmetehri		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

std::unordered_map<std::string, std::unordered_set<int>> channels;
std::unordered_map<int, std::string> client_nicknames;
std::unordered_map<int, bool> client_authenticated;

const std::string SERVER_PASSWORD = "secretpassword"; // Set your server password here

void handle_client(int client_socket, fd_set& read_fds) {
	char buffer[1024] = {0};
	int valread = read(client_socket, buffer, 1024);
	if (valread <= 0) {
		// Client disconnected
		close(client_socket);
		FD_CLR(client_socket, &read_fds);
		for (auto& channel : channels) {
			channel.second.erase(client_socket);
		}
		client_nicknames.erase(client_socket);
		client_authenticated.erase(client_socket);
		std::cout << "Client disconnected: " << client_socket << std::endl;
		return;
	}

	std::string message(buffer, valread);
	std::cout << "Received: " << message << std::endl;

	if (!client_authenticated[client_socket]) {
		if (message.substr(0, 4) == "PASS") {
			std::string password = message.substr(5);
			password.erase(password.find_last_not_of(" \n\r\t") + 1);
			if (password == SERVER_PASSWORD) {
				client_authenticated[client_socket] = true;
				std::string response = "Password accepted. You are now authenticated.\n";
				send(client_socket, response.c_str(), response.size(), 0);
			} else {
				std::string response = "Invalid password. Disconnecting...\n";
				send(client_socket, response.c_str(), response.size(), 0);
				close(client_socket);
				FD_CLR(client_socket, &read_fds);
				client_authenticated.erase(client_socket);
				std::cout << "Client failed authentication: " << client_socket << std::endl;
			}
		} else {
			std::string response = "You must authenticate first. Send PASS <password>.\n";
			send(client_socket, response.c_str(), response.size(), 0);
		}
		return;
	}

	if (message.substr(0, 4) == "NICK") {
		std::string nickname = message.substr(5);
		nickname.erase(nickname.find_last_not_of(" \n\r\t") + 1);
		client_nicknames[client_socket] = nickname;
		std::string response = "NICK set to " + nickname + "\n";
		send(client_socket, response.c_str(), response.size(), 0);
	} else if (message.substr(0, 5) == "JOIN") {
		std::string channel = message.substr(6);
		channel.erase(channel.find_last_not_of(" \n\r\t") + 1);
		channels[channel].insert(client_socket);
		std::string response = "Joined channel " + channel + "\n";
		send(client_socket, response.c_str(), response.size(), 0);
	} else if (message.substr(0, 4) == "PART") {
		std::string channel = message.substr(5);
		channel.erase(channel.find_last_not_of(" \n\r\t") + 1);
		channels[channel].erase(client_socket);
		std::string response = "Left channel " + channel + "\n";
		send(client_socket, response.c_str(), response.size(), 0);
	} else if (message.substr(0, 4) == "QUIT") {
		close(client_socket);
		FD_CLR(client_socket, &read_fds);
		for (auto& channel : channels) {
			channel.second.erase(client_socket);
		}
		client_nicknames.erase(client_socket);
		client_authenticated.erase(client_socket);
		std::cout << "Client quit: " << client_socket << std::endl;
	} else {
		std::string channel = message.substr(0, message.find(' '));
		std::string msg = message.substr(message.find(' ') + 1);
		for (int client : channels[channel]) {
			if (client != client_socket) {
				std::string response = client_nicknames[client_socket] + ": " + msg + "\n";
				send(client, response.c_str(), response.size(), 0);
			}
		}
	}
}

int main() {
	int server_socket, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(6667);

	if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_socket, 10) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	std::cout << "IRC Server is running on port 6667..." << std::endl;

	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(server_socket, &read_fds);
	int max_fd = server_socket;

	while (true) {
		fd_set tmp_fds = read_fds;
		int activity = select(max_fd + 1, &tmp_fds, nullptr, nullptr, nullptr);

		if (activity < 0) {
			perror("select error");
			continue;
		}

		if (FD_ISSET(server_socket, &tmp_fds)) {
			if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}

			std::cout << "New client connected: " << new_socket << std::endl;

			FD_SET(new_socket, &read_fds);
			if (new_socket > max_fd) {
				max_fd = new_socket;
			}

			// Initialize client state
			client_authenticated[new_socket] = false;
			std::string response = "Welcome to the IRC server. Please authenticate with PASS <password>.\n";
			send(new_socket, response.c_str(), response.size(), 0);
		}

		for (int fd = 0; fd <= max_fd; fd++) {
			if (fd != server_socket && FD_ISSET(fd, &tmp_fds)) {
				handle_client(fd, read_fds);
			}
		}
	}

	return 0;
}
