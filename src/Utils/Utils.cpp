/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:57:48 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/06 13:01:04 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../../inc/Const.hpp"

namespace Utils {
	int							sendData(int socketFd, const std::string& message) {
		ssize_t	bytesSent = send(socketFd, message.c_str(), message.length(), 0);
		if (bytesSent == -1) {
			perror("Error: Couldn't send data to the recepient socket\n");
			return (-1);
		}
		return 0;
	}

	int							receiveData(int socketFd, char* buffer, int bufferSize) {
		ssize_t	bytesReceived = recv(socketFd, buffer, bufferSize - 1, 0);
		if (bytesReceived == -1) {
			perror("Error: Couldn't receive data from the sender Socket\n");
			return (-1);
		}
		if (bytesReceived == 0) {
			return (0);
		}
		buffer[bytesReceived] = '\0';
		return bytesReceived;
	}

	std::string					trimString( const std::string& str ) {
		size_t	first = str.find_first_not_of(' ');
		if (std::string::npos == first) {
			return str;
		}
		size_t	last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
	}

	std::vector<std::string>	splitString(const std::string& s, const std::string& delimiter) {
		std::vector<std::string>	tokens;
		size_t						start = 0;
		size_t						end = s.find(delimiter);

		while (end != std::string::npos) {
			tokens.push_back(s.substr(start, end - start));
			start = end + delimiter.length();
			end = s.find(delimiter, start);
		}
		tokens.push_back(s.substr(start, end));
		return (tokens);
	}
	int								parseStringToInt(const std::string& str) {
		char* endptr;
		const char* cstr = str.c_str();
		long result = strtol(cstr, &endptr, 10);

		if (cstr == endptr) {
			throw std::invalid_argument("Invalid integer: " + str);
		} else if (result < INT32_MIN || result > INT32_MAX) {
				throw std::out_of_range("Integer out of range: " + str);
			}
			return static_cast<int>(result);
		}
}

