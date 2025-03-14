/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Colors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 09:32:34 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/10 09:50:43 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __COLORS_HPP__
# define __COLORS_HPP__

#include <iostream>
#include <string>

namespace Colors {


void			printColored(const std::string& color, const std::string& text, bool newline);
std::string		returnColored(const std::string& color, const std::string& text, bool newline);

} // namespace Colors






# endif
