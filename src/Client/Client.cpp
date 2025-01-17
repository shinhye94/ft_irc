/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:21:59 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/17 14:45:52 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client.hpp"

Client::Client()
{
	// char*	ip = "232.143.10.24"

	// this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	// address.sin_port = 5555;
	// address.sin_family = AF_INET;
	// inet_pton(AF_INET, ip, address.sin_addr.s_addr);

}

void Client::initSock()
{
	// int result = connect(this->_fd, &address, sizeof(adress));
	// if (result == 0)
		std::cout << "connected successfully !" << std::endl;
}
