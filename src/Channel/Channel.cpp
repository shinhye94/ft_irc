/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 01:44:40 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 12:57:45 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"

								Channel::Channel(const std::string& name) : _name(name), _topic(""), _topicLocked(false), _inviteOnly(false), _key(""), _userLimit(0) {};

								Channel::~Channel() {};

const std::string&				Channel::getName( void ) const {
	return _name;
}

bool 							Channel::addUser(Client* client) {
	if (hasUserLimit() && isLimitReached()) {
		return (false);
	}
	if (_users.find(client) == _users.end()) {
		_users.insert(client);
		return (true);
	}
	return (false);
}

bool							Channel::removeUser(Client* client) {
	if (_users.erase(client)) {
		_operators.erase(client);
		return (true);
	}
	return (false);
}

bool							Channel::hasUser(Client* client) const {
	return (_users.count(client));
}

size_t							Channel::getUserCount( void ) const {
	return (_users.size());
}

const std::set<Client*>& 		Channel::getUsers( void ) const {
	return (_users);
}

void							Channel::broadcastMessage(const std::string& message, Client* sender) {
	std::set<Client*>::iterator		it;
	for (it = _users.begin(); it != _users.end(); it++) {
		Client*	currentClient = *it;
		if (currentClient != sender) {
			currentClient->sendMessage(message);
		}
	}
}

void							Channel::setTopic(const std::string& topic) {
	this->_topic = topic;
}

const std::string&				Channel::getTopic( void ) const {
	return (_topic);
}

bool							Channel::isOperator(Client* client) const {
	return (_operators.count(client));
}

bool							Channel::addOperator(Client* client) {
	return (_operators.insert(client).second);
}

bool							Channel::removeOperator(Client* client) {
	return (_operators.erase(client));
}

bool							Channel::isInviteOnly( void ) const {
	return (_inviteOnly);
}

void							Channel::setInviteOnly(bool inviteOnly) {
	this->_inviteOnly = inviteOnly;
}

bool							Channel::hasKey( void ) const {
	return (!_key.empty());
}

void							Channel::setKey( const std::string& key ) {
	this->_key = key;
}

const std::string&				Channel::getKey ( void ) const {
	return (_key);
}

bool							Channel::checkKey(const std::string& key) const {
	return (this->_key == key);
}

void							Channel::clearKey( void ) {
	this->_key = "";
}

bool							Channel::hasUserLimit( void ) const {
	return (_userLimit > 0);
}

void							Channel::setUserLimit(int limit) {
	this->_userLimit = limit;
}

int								Channel::getUserLimit( void ) const {
	return (this->_userLimit);
}

bool							Channel::isLimitReached( void ) const {
	return (hasUserLimit() && getUserCount() >= _userLimit);
}

void							Channel::clearUserLimit( void ) {
	this->_userLimit = 0;
}

bool							Channel::isTopicLocked( void) const {
	return (_topicLocked);
}

void							Channel::setTopicLocked(bool topicLocked) {
	this->_topicLocked = topicLocked;
}

bool							Channel::isInvited(const std::string& nickname) const {
	return (_invitedUsers.count(nickname));
}

void							Channel::inviteUser(const std::string& nickname) {
	_invitedUsers.insert(nickname);
}

void							Channel::clearInvites( void ) {
	_invitedUsers.clear();
}
