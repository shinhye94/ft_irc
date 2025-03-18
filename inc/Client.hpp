/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:09:24 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 06:51:34 by bmetehri         ###   ########.fr       */
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
		std::string						_hostname;
		std::string						_servername;
		std::string						_realname;
		bool							_authenticated;
		std::string						_inputBuffer;

		bool						_toBeRemoved;

	public:
										Client(int socketFD);
										~Client( void );

		int								getSocketFD( void ) const;
		const std::string&				getNickname( void ) const;
		void							setNickname(const std::string& nickname);
		const std::string&				getUsername( void ) const;
		const std::string& 				getHostname( void ) const;
		const std::string& 				getServername( void ) const;
		const std::string& 				getRealname( void ) const;
		void							setNamesAndStuff(const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname);
		bool							isAuthenticated( void) const;
		void							setAuthenticated(bool authenticated);

		void							sendMessage(const std::string& message);
		int								receiveData(char* buffer, int bufferSize);
		void							disconnect( void );

		bool							isToBeRemoved() const;
		void							setRemoveSituation(bool removeSituation);

		std::string						getBuffer( void );
		void							setBuffer(const std::string& buffer);
		void							clearBuffer( void );
		void							appendToBuffer(const std::string& data);
};

#endif
