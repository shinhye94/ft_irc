/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:09:24 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/17 14:44:05 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

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

class Client {
	private:
		int						_fd;
		struct sockaddr_in		address;
		std::string				_username;
		std::string				_nickname;

	public:
		Client();
		void initSock();

};

#endif
