# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/15 13:33:10 by bmetehri          #+#    #+#              #
#    Updated: 2025/01/17 15:06:08 by bmetehri         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


#****************************************************************************#
#								VARIABLES									#
#****************************************************************************#


CXX = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98 -Wshadow

RM = rm -rf

INCLUDE = inc
INCLUDES = -I $(INCLUDE)

OBJ = obj/

TARGET = ircserv


#****************************************************************************#
#								SOURCES										#
#****************************************************************************#

CLIENT_SRC = Client/
CLIENT_FILES =	Client.cpp

SERVER_SRC = Server/
SERVER_FILES = Server.cpp

UTILS_SRC = Utils/
UTILS_FILES =	Utils.cpp

IRCSERV_SRC = src/
IRCSERV_FILES = main.cpp	\
				${addprefix ${SERVER_SRC}, ${SERVER_FILES}}	\
				${addprefix ${CLIENT_SRC}, ${CLIENT_FILES}}	\
				${addprefix ${UTILS_SRC}, ${UTILS_FILES}}

IRCOBJ = ${addprefix ${OBJ}, ${IRCSERV_FILES:.cpp=.opp}}
#****************************************************************************#
#								COLORS										#
#****************************************************************************#

define BLACK
	@echo -e "\033[0;30m$1\033[0m"
endef

define RED
	@echo -e "\033[0;31m$1\033[0m"
endef

define GREEN
	@echo -e "\033[0;32m$1\033[0m"
endef

define YELLOW
	@echo -e "\033[0;33m$1\033[0m"
endef

define BLUE
	@echo -e "\033[0;34m$1\033[0m"
endef

define PURPLE
	@echo -e "\033[0;35m$1\033[0m"
endef

define CYAN
	@echo -e "\033[0;36m$1\033[0m"
endef

#****************************************************************************#
#								REGLES										#
#****************************************************************************#

all: $(TARGET)

$(TARGET): $(IRCOBJ)
	$(call GREEN, "Compiling ft_irc...")
	$(CXX) $(CFLAGS) $(IRCOBJ) $(INCLUDES) -o $(TARGET)
	$(call GREEN, "ft_irc compiled!")

$(OBJ)%.opp: $(IRCSERV_SRC)%.cpp
	@mkdir -p $(@D)
	$(call CYAN, "Compiling $<...")
	@$(CXX) $(CFLAGS) -c $< -o $@

clean:
	$(call RED, "Cleaning ft_irc object files...")
	@$(RM) $(OBJ)
	$(call RED, "ft_irc object files cleaned!")

fclean: clean
	$(call RED, "Cleaning ft_irc...")
	@$(RM) $(TARGET)
	$(call RED, "ft_irc cleaned!")

vg: re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=default.supp ./ircserv

re: fclean all
