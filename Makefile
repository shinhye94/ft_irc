# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bmetehri <bmetehri@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/15 13:33:10 by bmetehri          #+#    #+#              #
#    Updated: 2025/03/06 12:40:42 by bmetehri         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


#****************************************************************************#
#								VARIABLES									#
#****************************************************************************#


#****************************************************************************#
#								VARIABLES									#
#****************************************************************************#

# Compiler
CXX = c++

# Compilation flags with dependency generation
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -Wshadow -MMD

# Remove command
RM = rm -rf

# Include directories
INCLUDE = inc
INCLUDES = -I $(INCLUDE)

# Object directory
OBJ = obj/

# Target executable
TARGET = ircserv

#****************************************************************************#
#								SOURCES										#
#****************************************************************************#

# Source directories
CLIENT_SRC = Client/
SERVER_SRC = Server/
CHANNEL_SRC = Channel/
UTILS_SRC = Utils/
IRCSERV_SRC = src/

# Source files
CLIENT_FILES = Client.cpp

CHANNEL_FILES = Channel.cpp

SERVER_FILES =		Server.cpp			\
					HandleClient.cpp	\
					HandleCommand.cpp	\
					HandleMessage.cpp
UTILS_FILES = Utils.cpp
IRCSERV_FILES = main.cpp \
                $(addprefix $(SERVER_SRC), $(SERVER_FILES)) \
                $(addprefix $(CLIENT_SRC), $(CLIENT_FILES)) \
				$(addprefix $(CHANNEL_SRC), $(CHANNEL_FILES)) \
                $(addprefix $(UTILS_SRC), $(UTILS_FILES))

# Object files
IRCOBJ = $(addprefix $(OBJ), $(IRCSERV_FILES:.cpp=.opp))

# Dependency files
DEPS = $(IRCOBJ:.opp=.d)

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
#								RULES										#
#****************************************************************************#

# Phony targets
.PHONY: all clean fclean re vg

# Default target: build the executable
all: $(TARGET)

# Rule to link object files into the target executable
$(TARGET): $(IRCOBJ)
	$(call GREEN, "Linking $(TARGET)...")
	$(CXX) $(CXXFLAGS) $(IRCOBJ) $(INCLUDES) -o $(TARGET)
	$(call GREEN, "$(TARGET) compiled successfully!")

# Pattern rule to compile source files into object files
$(OBJ)%.opp: $(IRCSERV_SRC)%.cpp
	@mkdir -p $(@D)
	$(call CYAN, "Compiling $<...")
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule: remove object files and dependency files
clean:
	$(call YELLOW, "Cleaning object files...")
	$(RM) $(OBJ)
	$(call YELLOW, "Object files cleaned!")

# Full clean rule: remove object files, dependency files, and the executable
fclean: clean
	$(call YELLOW, "Cleaning executable...")
	$(RM) $(TARGET)
	$(call YELLOW, "$(TARGET) cleaned!")

# Rebuild rule: clean everything and rebuild from scratch
re: fclean all

# Valgrind rule: rebuild and run the program under Valgrind
vg: re
	$(call RED, "Running Valgrind...")
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=default.supp ./$(TARGET)

# Include dependency files
-include $(DEPS)
