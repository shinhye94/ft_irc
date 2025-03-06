/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:21:59 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 04:14:28 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Utils.hpp"
#include "../../inc/Client.hpp"


								Client::Client(int socketFD) : _socketFD(socketFD), _nickname(""), _username(""), _authenticated(false) {};

								Client::~Client( void ) {};

int								Client::getSocketFD( void ) const {
	return (_socketFD);
}

const std::string& 				Client::getNickname( void ) const {
	return (this->_nickname);
}

void							Client::setNickname(const std::string& nickname) {
	this->_nickname = nickname;
}

const std::string& 				Client::getUsername( void ) const {
	return (this->_username);
}

void							Client::setUsername(const std::string& username) {
	this->_username = username;
}

bool							Client::isAuthenticated( void ) const {
	return (_authenticated);
}

void							Client::setAuthenticated(bool authenticated) {
	this->_authenticated = authenticated;
}

void							Client::sendMessage(const std::string& message) {
	Utils::sendData(_socketFD, message);
}

int								Client::receiveData(char* buffer, int bufferSize) {
	return Utils::receiveData(_socketFD, buffer, bufferSize);
}

void							Client::disconnect() {
	close(_socketFD);
	_socketFD = -1;
}
