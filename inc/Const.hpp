/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Const.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 13:58:31 by bmetehri          #+#    #+#             */
/*   Updated: 2025/03/18 02:46:40 by bmetehri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	__CONSTS_HPP__
# define	__CONSTS_HPP__
#include "Import.h"


# ifndef PORT
#  define PORT 6667 // Default IRC port
# endif

# ifndef MAX_CLIENTS
#  define MAX_CLIENTS 10
# endif

# define	NEWLINE		true
# define	NOBEWLINE	false

#define REUSEADDR_OPTION 1

// ANSI color codes for debugging
# define	RED		"\033[31m"
# define	GREEN	"\033[32m"
# define	YELLOW	"\033[33m"
# define	BLUE	"\033[34m"
# define	MAGENTA	"\033[35m"
# define	CYAN	"\033[36m"
# define	WHITE	"\033[37m"
# define	RESET	"\033[0m"

#endif	//	__CONSTS_HPP__
