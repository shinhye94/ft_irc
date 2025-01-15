# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/15 13:33:10 by bmetehri          #+#    #+#              #
#    Updated: 2025/01/15 13:58:26 by bmetehri         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



CXX = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98 -Wshadow

TARGET = ircserv

IRCSERV_SRCS = src/
IRCSERV_FILES = main.cpp
