/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:57:48 by bmetehri          #+#    #+#             */
/*   Updated: 2025/01/17 14:46:24 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../../inc/Consts.hpp"

void	logInColor(char* color, std::string text)
{
	std::cout << color << text << RESET;
}
