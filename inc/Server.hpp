/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:10:00 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 12:47:04 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__

#include "Utils.hpp"
#include "Client.hpp"


class Server {
	private:

/*#****************************************************************************#
#								VARS											#
#****************************************************************************#*/
		int										_serverSocket;	// a socket representing the server
		int										_portNumber;		// a port to listen to
		std::string								_serverPassword;	// a password to authenteficate users

		std::set<Client*>						_clients;
		std::map<int, Client*>					_clientMap;

		std::map<std::string, Channel>			_channels;		// channels name -> set of clients sockets

		fd_set									_readfds;		// master set for select()
		int										_fdMax;			// highest file descriptor number

/*#****************************************************************************#
#								HandleCLient									#
#****************************************************************************#*/
		void									setupServerSocket( void );
		void									acceptNewClient( void );
		void									handleClientData( Client* client );
		void									removeClient( Client* client );

		Client*									findClientBySocket( int socketFD);
		Client*									findClientByNickname( const std::string& nickname );
		Channel*								findChannel(const std::string& channelName);

/*#****************************************************************************#
#								HandleCommand									#
#****************************************************************************#*/
		void									processCommand( Client* client, const std::string& commandLine );

		void									handlePASS( Client* client, const std::vector<std::string>& params );
		void									handleNICK( Client* client, const std::vector<std::string>& params );
		void									handleUSER( Client* client, const std::vector<std::string>& params );
		void									handleJOIN( Client* client, const std::vector<std::string>& params );
		void									handlePART( Client* client, const std::vector<std::string>& params );
		void									handlePRIVMSG( Client* client, const std::vector<std::string>& params );
		void									handleQUIT( Client* client );
		void									handleMODE( Client* client, const std::vector<std::string>& params );
		void									handleWHOIS( Client* client, const std::vector<std::string>& params );
		void									handleWHO( Client* client, const std::vector<std::string>& params );
		void									handlePING( Client* client, const std::vector<std::string>& params );
		void									handleKICK( Client* client, const std::vector<std::string>& params );
		void									handleINVITE( Client* client, const std::vector<std::string>& params );
		void									handleTOPIC( Client* client, const std::vector<std::string>& params );


/*#****************************************************************************#
#								HandleMessage									#
#****************************************************************************#*/
		void									broadcastToChannel( const std::string& channelName, const std::string& message, Client* sender = 0 );
		void									sendToClient( Client* client, const std::string& message );
		std::string								getClientNickname( Client* client );
		bool									isNicknameInUse( const std::string& nickname );
		bool									isClientAuthenticated( Client* client );
		bool									authenticateClient( Client* client, const std::string& passwordAttempt );

	public:
												Server( int port, const std::string& password );
												~Server( void );

		void									run( void );
};

#endif
