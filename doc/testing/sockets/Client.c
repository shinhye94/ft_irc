/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 18:21:59 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/17 13:50:25 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <iostream>
// #include <string>
// #include <vector>
// #include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
// #include <cstring>
// #include "Sockets.hpp"

// int	main(int ac, char** av) {
// 	sockaddr_in	serverAdress;

// 	int	clientSockets = socket( AF_INET, SOCK_STREAM, 0);
// 	serverAdress.sin_family = AF_INET;
// 	serverAdress.sin_port = htons(5555);
// 	serverAdress.sin_addr.s_addr = INADDR_ANY;

// 	connect(clientSockets, (struct sockaddr *)&serverAdress, sizeof(serverAdress));

// 	const char* message = "Hello, server!";
// 	send(clientSockets, message, strlen(message), 0);

// 	close(clientSockets);
// 	return (0);
// }

int	main(void)
{
	char*	ip = "142.251.163.27";
	int	_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in	address;

	address.sin_port = htons(80);
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr.s_addr);


	int result = connect(_fd, (struct sockaddr *)&address, sizeof(address));
	if (result == 0)
		printf("connected successfully\n");
	return (0);
}
