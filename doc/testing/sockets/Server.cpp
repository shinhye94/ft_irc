/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 18:22:13 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/16 19:04:36 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
// #include "Sockets.hpp"

int	main(int ac, char** av) {
	sockaddr_in	serverAdress;

	int	serverSocket = socket( AF_INET, SOCK_STREAM, 0);
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(5555);
	serverAdress.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr* )&serverAdress, sizeof(serverAdress));

	listen(serverSocket, 5);


	int clientSockets = accept(serverSocket, nullptr, nullptr);

	char buf[1024] = { 0 };
	recv(clientSockets, buf, sizeof(buf), 0);
	std::cout << "message from client socket: " << buf << std::endl;

	close(serverSocket);
	return (0);
}