/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:10:00 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/22 14:19:40 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__

#include "Client.hpp"


class Server {
	private:
		int							_port;
		std::vector<Client>			_clients;
		std::vector<struct pollFd>	_pollFds;
		int							serverSocket;
	public:
		Server();
		void	handleClient(int clientSocket);
};

#endif
