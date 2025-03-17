/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:09:24 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 00:44:35 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__
#include "Channel.hpp"

class Client {
	private:
		int								_socketFD;
		std::string						_nickname;
		std::string						_username;
		bool							_authenticated;

		bool						_toBeRemoved;

	public:
										Client(int socketFD);
										~Client( void );

		int								getSocketFD( void ) const;
		const std::string&				getNickname( void ) const;
		void							setNickname(const std::string& nickname);
		const std::string&				getUsername( void ) const;
		void							setUsername(const std::string& username);
		bool							isAuthenticated( void) const;
		void							setAuthenticated(bool authenticated);

		void							sendMessage(const std::string& message);
		int								receiveData(char* buffer, int bufferSize);
		void							disconnect( void );

		bool							isToBeRemoved() const;
		void							setRemoveSituation(bool removeSituation);
};

#endif
