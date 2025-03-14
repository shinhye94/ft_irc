/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 00:14:49 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/10 09:50:08 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace Utils {

/**
 * @brief Sends data to a socket.
 * @param socket_fd Socket file descriptor.
 * @param message Message to send.
 * @return 0 on success, -1 on failure.
 */
int sendData(int socket_fd, const std::string& message);

/**
 * @brief Receives data from a socket.
 * @param socket_fd Socket file descriptor.
 * @param buffer Buffer to store received data.
 * @param buffer_size Size of the buffer.
 * @return Number of bytes received, -1 on error, 0 if connection closed.
 */
int receiveData(int socket_fd, char* buffer, int buffer_size);

/**
 * @brief Splits a string by a delimiter.
 * @param s String to split.
 * @param delimiter Delimiter.
 * @return Vector of strings.
 */
std::vector<std::string> splitString(const std::string& s, const std::string& delimiter);

/**
 * @brief Trims leading and trailing whitespaces from a string.
 * @param str String to trim.
 * @return Trimmed string.
 */
std::string trimString(const std::string& str);


int								parseStringToInt(const std::string& str);

} // namespace Utils

#endif // UTILS_HPP
