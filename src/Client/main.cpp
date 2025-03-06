/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 03:14:06 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/02 03:53:04 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Utils.hpp"
#include "../../inc/Client.hpp"
#include <iostream>
#include <sstream>
// #include <thread>

int		main(int argc, char** argv) {
	std::string				serverAddress = "127.0.0.1";
	int						port = 5555;
	std::string				password = "password123";

	if (argc > 1) {
		serverAddress = argv[1];
	}
	if (argc > 2) {
		std::istringstream	iss(argv[2]);
		int					i;
		iss >> i;

		if (i <= 0) {
			std::cerr << "Invalid port number. Using default port " << port << std::endl;
		} else {
			port = i;
		}
	}

	Client	client(serverAddress, port, password);
	client.run();
	return (0);
}
