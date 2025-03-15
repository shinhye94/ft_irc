/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 15:12:59 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/15 14:24:57 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANNEL_HPP__
# define __CHANNEL_HPP__

#include "Const.hpp"

class Client;

class Channel {
	private:
		std::string					_name;
		std::set<Client *>			_users;
		std::set<Client *>			_operators;
		std::string					_topic;
		bool						_topicLocked;
		bool						_inviteOnly;
		bool						_toBeRemoved;
		bool						_justCreated;
		std::string					_key;
		size_t							_userLimit;
		std::set<std::string>		_invitedUsers;


	public:
									Channel(const std::string& name);
									~Channel( void );

		const std::string&			getName( void ) const;
		bool						addUser(Client* client);
		bool						removeUser(Client* client);
		bool						hasUser(Client* client) const;
		size_t						getUserCount( void ) const;
		const std::set<Client*>&	getUsers( void ) const;
		void						broadcastMessage(const std::string& message, Client* sender = 0);

		void						setTopic(const std::string& topic);
		const std::string&			getTopic( void ) const;

		bool						isOperator(Client* client) const;
		bool						addOperator(Client* client);
		bool						removeOperator(Client* client);

		bool						isInviteOnly( void ) const;
		void						setInviteOnly(bool inviteOnly);

		bool						hasKey() const;
		void						setKey(const std::string& key);
		const std::string&			getKey( void ) const;
		bool						checkKey(const std::string& key) const;
		void						clearKey( void );

		bool						hasUserLimit( void ) const;
		void						setUserLimit(int limit);
		int							getUserLimit( void ) const;
		bool						isLimitReached( void ) const;
		void						clearUserLimit( void );

		bool						isTopicLocked() const;
		void						setTopicLocked(bool topicLocked);

		bool						isInvited(const std::string& nickname) const;
		void						inviteUser(const std::string& nickname);
		void						clearInvites();

		bool						isToBeRemoved() const;
		void						setRemoveSituation(bool removeSituation);

		bool						getJustCreated() const;
		void						setJustCreated(bool justCreatedOrNot);
};


#endif
