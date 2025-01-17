/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:10:00 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/17 13:16:43 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__

#include "Client.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <cstring>

class Server {
	private:
		int			_port;
		std::vector<Client>	_clients;
	public:
};

#endif
