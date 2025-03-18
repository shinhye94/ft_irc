/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:21:59 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 06:52:14 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Utils.hpp"
#include "../../inc/Client.hpp"


								Client::Client(int socketFD) : _socketFD(socketFD), _nickname(""), _username(""), _authenticated(false), _toBeRemoved(false) {};

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

const std::string& 				Client::getHostname( void ) const {
	return (this->_hostname);
}

const std::string& 				Client::getRealname( void ) const {
	return (this->_realname);
}


const std::string& 				Client::getServername( void ) const {
	return (this->_servername);
}


void							Client::setNamesAndStuff(const std::string& username, const std::string& hostname, const std::string& servername, const std::string& realname) {
	this->_username = username;
	this->_hostname = hostname;
	this->_servername = servername;
	this->_realname = realname;
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


bool							Client::isToBeRemoved() const {
	return (this->_toBeRemoved);
}

void							Client::setRemoveSituation(bool removeSituation) {
	this->_toBeRemoved = removeSituation;
}

std::string						Client::getBuffer( void ) {
	return (this->_inputBuffer);
}

void							Client::setBuffer(const std::string& buffer) {
	_inputBuffer = buffer;
}

void							Client::clearBuffer( void ) {
	_inputBuffer.erase();
}

void							Client::appendToBuffer( const std::string& data) {
	_inputBuffer += data;
}
