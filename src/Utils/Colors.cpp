/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Colors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 02:32:59 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/10 09:55:43 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Colors.hpp"
#include "../../inc/Const.hpp"

namespace Colors {

	void			printColored(const std::string& color, const std::string& text, bool newline) {
		std::cout << color << text << RESET;
		if (newline) {
			std::cout << std::endl;
		}
	}

	std::string		returnColored(const std::string& color, const std::string& text, bool newline) {
		if (newline)
			return (color + text + RESET + "\n");
		return (color + text + RESET);
	}

}
