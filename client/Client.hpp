/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 01:49:02 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 01:51:19 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

#include "../inc/Import.h"

class Client {
	private:
		int					_clientSocket;
		std::string			_serverAddressStr;
		int					_portNumber;
		std::string			_clientPassword;
		std::string			_nickanme;
		std::string			_username;

		bool				connectToServer( void );
		void				handleServerResponse( void );
		void				handleUserInput( void );
		void				sendCommand( const std::string& command);
	public:
			Client(const std::string& serverAddress, int port, const std::string& password);
			~Client( void );
	void	run( void );

};

#endif
