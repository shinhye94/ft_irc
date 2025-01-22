/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:09:24 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/22 14:19:49 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__
#include "Channel.hpp"

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
